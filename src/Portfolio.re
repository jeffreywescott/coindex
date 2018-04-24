module Asset = {
  type t = {
    symbol: Symbol.t,
    balance: float,
  };
};

type t = array(Asset.t);