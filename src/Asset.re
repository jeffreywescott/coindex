type t = {
  symbol: string,
  balance: float,
};

let getCanonicalSymbol = (~symbol: string, ~host: Host.t) =>
  switch (symbol, host) {
  | ("BCC", Host.Binance) => "BCH"
  | _ => symbol
  };