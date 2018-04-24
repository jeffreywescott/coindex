let writeTradeList = () => {
  let {apiKey, secret}: Binance.Api.Credentials.t =
    Binance.Api.Credentials.load(Config.projectRoot);
  Js.Promise.(
    CoinMarketCap.Api.Fetchers.getTopCryptoCoins(
      CoinMarketCap.TopCoinsIndex.numCoins,
    )
    |> then_(json =>
         resolve(json |> CoinMarketCap.TopCoinsIndex.Decode.indexDecode)
       )
    |> then_(index =>
         Binance.Api.Fetchers.getAccountInfo(apiKey, secret)
         |> then_(json => {
              let assets = json |> Binance.Account.Decode.portfolioDecode;
              let portfolio =
                Js.Array.filter(
                  (asset: Portfolio.Asset.t) => asset.balance > 0.0,
                  assets,
                );
              resolve((index, portfolio));
            })
       )
    |> then_(indexAndPortfolio => {
         let (index, portfolio) = indexAndPortfolio;
         let tradeList = TradeList.fromIndexAndPortfolio(index, portfolio);
         resolve(tradeList);
       })
    |> then_(tradeList => resolve(tradeList |> TradeList.jsonEncode))
    |> then_(json => resolve(Json2Csv.json2csv(json)))
    |> then_(csv =>
         resolve(Node.Fs.writeFileSync(TradeList.filename, csv, `utf8))
       )
    |> ignore
  );
};

writeTradeList();