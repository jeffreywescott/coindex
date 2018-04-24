module Coin = {
  type t = {
    symbol: string,
    marketCap: float,
  };
};

type t = array(Coin.t);