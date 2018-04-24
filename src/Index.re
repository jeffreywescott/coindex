module Coin = {
  type t = {
    symbol: string,
    priceUsd: float,
    marketCapUsd: float,
  };
};

type t = array(Coin.t);

let computeMarketCapPercentages = (index: t) => {
  let totalMarketCapUsd =
    Js.Array.reduce(
      (total, coin: Coin.t) => total +. coin.marketCapUsd,
      0.0,
      index,
    );
  Js.Array.map(
    (coin: Coin.t) => coin.marketCapUsd /. totalMarketCapUsd,
    index,
  );
};