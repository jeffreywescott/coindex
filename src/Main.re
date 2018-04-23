module BinanceAccount = {
  type asset = {
    symbol: string,
    balance: float,
  };
  module Decode = {
    let assetDecode = json =>
      Json.Decode.{
        symbol: json |> field("asset", string),
        balance: json |> field("free", string) |> Js.Float.fromString,
      };
    let balancesDecode = json =>
      Json.Decode.(json |> field("balances", array(assetDecode)));
  };
  module Encode = {
    let assetEncode = r =>
      Json.Encode.object_([
        ("symbol", Json.Encode.string(r.symbol)),
        ("balance", Json.Encode.float(r.balance)),
      ]);
    let balancesEncode = balances =>
      balances |> Json.Encode.array(assetEncode);
  };
};

let writeSnapshots = () => {
  let {apiKey, secret}: Config.Binance.apiKeys =
    Config.Binance.getApiKeys(Config.projectRoot);
  Js.Promise.(
    Fetchers.getTopCryptoCoins(Config.CoinMarketCap.numCoins)
    |> then_(json => resolve(Json2Csv.json2csv(json)))
    |> then_(csv =>
         resolve(
           Node.Fs.writeFileSync(
             Config.CoinMarketCap.topCoinsFilename,
             csv,
             `utf8,
           ),
         )
       )
    |> ignore
  );
  Js.Promise.(
    Fetchers.getBinanceAccountInfo(apiKey, secret)
    |> then_(json => {
         let balances = json |> BinanceAccount.Decode.balancesDecode;
         resolve(
           balances
           |> Js.Array.filter((asset: BinanceAccount.asset) =>
                asset.balance > 0.0
              )
           |> BinanceAccount.Encode.balancesEncode,
         );
       })
    |> then_(json => resolve(Json2Csv.json2csv(json)))
    |> then_(csv =>
         resolve(
           Node.Fs.writeFileSync(Config.Binance.balancesFilename, csv, `utf8),
         )
       )
    |> ignore
  );
};

writeSnapshots();