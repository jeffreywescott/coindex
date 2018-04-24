let writeSnapshots = () => {
  let {apiKey, secret}: Binance.Api.credentials =
    Binance.Api.getCredentials(Config.projectRoot);
  Js.Promise.(
    CoinMarketCap.Api.Fetchers.getTopCryptoCoins(CoinMarketCap.numTopCoins)
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
           |> Js.Array.filter((asset: Binance.Account.asset) =>
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