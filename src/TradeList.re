module Trade = {
  type t = {
    symbol: Symbol.t,
    priceUsd: float,
    marketCapUsd: float,
    currentPercentage: float,
    targetPercentage: float,
    percentageDelta: float,
    balance: float,
    balanceDeltaUsd: float,
    balanceDelta: float,
  };
  let jsonEncode = (r: t) =>
    Json.Encode.object_([
      ("symbol", Json.Encode.string(r.symbol)),
      ("priceUsd", Json.Encode.float(r.priceUsd)),
      ("marketCapUsd", Json.Encode.float(r.marketCapUsd)),
      ("currentPercentage", Json.Encode.float(r.currentPercentage)),
      ("targetPercentage", Json.Encode.float(r.targetPercentage)),
      ("percentageDelta", Json.Encode.float(r.percentageDelta)),
      ("balance", Json.Encode.float(r.balance)),
      ("balanceDeltaUsd", Json.Encode.float(r.balanceDeltaUsd)),
      ("balanceDelta", Json.Encode.float(r.balanceDelta)),
    ]);
};

exception CoinPriceError(string);

type t = array(Trade.t);

let jsonEncode = tradeList =>
  tradeList |> Json.Encode.array(Trade.jsonEncode);

let fetchIndexAndPortfolio = () => {
  let {apiKey, secret}: Binance.Api.Credentials.t =
    Binance.Api.Credentials.load(Config.projectRoot);
  Js.Promise.(
    all2((
      CoinMarketCap.Api.Fetchers.getTopCryptoCoins(15),
      Binance.Api.Fetchers.getPortfolio(apiKey, secret),
    ))
    |> then_(responses => {
         let (topSymbols, portfolio) = responses;
         let portfolioSymbols =
           Js.Array.map(
             (asset: Portfolio.Asset.t) => asset.symbol,
             portfolio,
           );
         let allSymbols = Js.Array.concat(topSymbols, portfolioSymbols);
         let uniqueSymbols =
           Js.Array.filteri(
             (symbol, i) => Js.Array.indexOf(symbol, allSymbols) == i,
             allSymbols,
           );
         CryptoCompare.Api.Fetchers.getIndex(uniqueSymbols)
         |> then_(index => resolve((index, portfolio)));
       })
  );
};

let fromIndexAndPortfolio = (index: Index.t, portfolio: Portfolio.t) : t => {
  let indexMarketCapUsd =
    Js.Array.reduce(
      (total, coin: Index.Coin.t) => total +. coin.marketCapUsd,
      0.0,
      index,
    );
  let indexMarketCapPercentagesBySymbol =
    Js.Dict.fromArray(
      Js.Array.map(
        (coin: Index.Coin.t) => (
          coin.symbol,
          coin.marketCapUsd /. indexMarketCapUsd,
        ),
        index,
      ),
    );
  let pricesBySymbol =
    Js.Dict.fromArray(
      Js.Array.map(
        (coin: Index.Coin.t) => (coin.symbol, coin.priceUsd),
        index,
      ),
    );
  let portfolioBalancesBySymbol =
    Js.Dict.fromArray(
      Js.Array.map(
        (asset: Portfolio.Asset.t) => (asset.symbol, asset.balance),
        portfolio,
      ),
    );
  let portfolioBalanceValuesUsdBySymbol =
    Js.Dict.fromArray(
      Js.Array.map(
        (asset: Portfolio.Asset.t) => {
          let price =
            switch (Js.Dict.get(pricesBySymbol, asset.symbol)) {
            | Some(p) => p
            | None =>
              raise(
                CoinPriceError({|Failed to get price for $(asset.symbol)|}),
              )
            };
          (asset.symbol, asset.balance *. price);
        },
        portfolio,
      ),
    );
  let portfolioTotalBalanceUsd =
    Js.Array.reduce(
      (total, value) => total +. value,
      0.0,
      Js.Dict.values(portfolioBalanceValuesUsdBySymbol),
    );
  let tradeList =
    Js.Array.map(
      (coin: Index.Coin.t) => {
        let portfolioPercentage =
          switch (Js.Dict.get(portfolioBalanceValuesUsdBySymbol, coin.symbol)) {
          | Some(bal) => bal /. portfolioTotalBalanceUsd
          | None => 0.0
          };
        let indexMarketCapPercentage =
          switch (Js.Dict.get(indexMarketCapPercentagesBySymbol, coin.symbol)) {
          | Some(p) => p
          | None => 0.0
          };
        let portfolioPercentageDelta =
          indexMarketCapPercentage -. portfolioPercentage;
        let portfolioBalance =
          switch (Js.Dict.get(portfolioBalancesBySymbol, coin.symbol)) {
          | Some(bal) => bal
          | None => 0.0
          };
        let portfolioBalanceDeltaUsd =
          portfolioTotalBalanceUsd *. portfolioPercentageDelta;
        let trade: Trade.t = {
          symbol: coin.symbol,
          priceUsd: coin.priceUsd,
          marketCapUsd: coin.marketCapUsd,
          currentPercentage: portfolioPercentage,
          targetPercentage: indexMarketCapPercentage,
          percentageDelta: portfolioPercentageDelta,
          balance: portfolioBalance,
          balanceDeltaUsd: portfolioBalanceDeltaUsd,
          balanceDelta: portfolioBalanceDeltaUsd /. coin.priceUsd,
        };
        trade;
      },
      index,
    );
  Js.Array.filter((trade: Trade.t) => trade.priceUsd > 0.0, tradeList);
};

let filename =
  Node.Path.join([|Config.projectRoot, "tmp", "trade-list.csv"|]);