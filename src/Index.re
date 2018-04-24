module Coin = {
  type t = {
    symbol: Symbol.t,
    priceUsd: float,
    marketCapUsd: float,
  };
};

type t = array(Coin.t);