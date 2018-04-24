let writeSnapshots = () => {
  let {apiKey, secret}: Binance.Api.Credentials.t =
    Binance.Api.Credentials.load(Config.projectRoot);
  Js.Promise.(
    CoinMarketCap.Api.Fetchers.getTopCryptoCoins(
      CoinMarketCap.TopCoinsIndex.numCoins,
    )
    |> then_(json => {
         let index = json |> CoinMarketCap.TopCoinsIndex.Decode.indexDecode;
         resolve(index |> CoinMarketCap.TopCoinsIndex.Encode.indexEncode);
       })
    |> then_(json => resolve(Json2Csv.json2csv(json)))
    |> then_(csv =>
         resolve(
           Node.Fs.writeFileSync(CoinMarketCap.topCoinsFilename, csv, `utf8),
         )
       )
    |> ignore
  );
  Js.Promise.(
    Binance.Api.Fetchers.getAccountInfo(apiKey, secret)
    |> then_(json => {
         let balances = json |> Binance.Account.Decode.balancesDecode;
         resolve(
           balances
           |> Js.Array.filter((asset: Portfolio.Asset.t) =>
                asset.balance > 0.0
              )
           |> Binance.Account.Encode.balancesEncode,
         );
       })
    |> then_(json => resolve(Json2Csv.json2csv(json)))
    |> then_(csv =>
         resolve(Node.Fs.writeFileSync(Binance.balancesFilename, csv, `utf8))
       )
    |> ignore
  );
};

writeSnapshots();