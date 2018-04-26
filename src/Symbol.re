type t = string;

let getCanonical = (~symbol: t, ~host: Host.t) =>
  switch (symbol, host) {
  | ("BCC", Host.Binance) => "BCH"
  | ("IOTA", Host.Binance) => "IOT"
  | ("MIOTA", Host.CoinMarketCap) => "IOT"
  | _ => symbol
  };