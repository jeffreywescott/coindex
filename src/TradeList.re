module Trade = {
  type t = {
    symbol: Symbol.t,
    priceUsd: float,
    currentPercentage: float,
    targetPercentage: float,
    percentageDelta: float,
    balanceDeltaUsd: float,
    balanceDelta: float,
  };
  let jsonEncode = (r: t) =>
    Json.Encode.object_([
      ("symbol", Json.Encode.string(r.symbol)),
      ("priceUsd", Json.Encode.float(r.priceUsd)),
      ("currentPercentage", Json.Encode.float(r.currentPercentage)),
      ("targetPercentage", Json.Encode.float(r.targetPercentage)),
      ("percentageDelta", Json.Encode.float(r.percentageDelta)),
      ("balanceDeltaUsd", Json.Encode.float(r.balanceDeltaUsd)),
      ("balanceDelta", Json.Encode.float(r.balanceDelta)),
    ]);
};

type t = array(Trade.t);

let jsonEncode = tradeList =>
  tradeList |> Json.Encode.array(Trade.jsonEncode);

/** FIXME: this function has a bug. It should trade away any coins that are
not in the index, but it doesn't. */
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
  let portfolioBalanceValuesUsdBySymbol =
    Js.Dict.fromArray(
      Js.Array.map(
        (asset: Portfolio.Asset.t) => {
          let price =
            switch (Js.Dict.get(pricesBySymbol, asset.symbol)) {
            | Some(p) => p
            | None => 0.0
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
    Js.Array.mapi(
      (coin: Index.Coin.t, idx) => {
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
        let portfolioBalanceDeltaUsd =
          portfolioTotalBalanceUsd *. portfolioPercentageDelta;
        let trade: Trade.t = {
          symbol: coin.symbol,
          priceUsd: coin.priceUsd,
          currentPercentage: portfolioPercentage,
          targetPercentage: indexMarketCapPercentage,
          percentageDelta: portfolioPercentageDelta,
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