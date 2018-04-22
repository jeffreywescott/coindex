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