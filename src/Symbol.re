type t = string;

let getCanonical = (~symbol: t, ~host: Host.t) =>
  switch (symbol, host) {
  | ("BCC", Host.Binance) => "BCH"
  | _ => symbol
  };