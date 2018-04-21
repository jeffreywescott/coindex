module CoinMarketCap = {
  let baseUrl = "https://api.coinmarketcap.com/v1";
  let tickerUrl = (numCoins: int) => {j|$(baseUrl)/ticker/?limit=$(numCoins)|j};
};

module Binance = {
  let baseUrl = "https://api.binance.com/api/v3";
  type apiKeys = {
    apiKey: string,
    secret: string,
  };
  module Decode = {
    let apiKeys = json =>
      Json.Decode.{
        apiKey: json |> field("apiKey", string),
        secret: json |> field("secret", string),
      };
  };
  let getApiKeys = projectRoot => {
    let binanceKeysFilename = Node.Path.join([|projectRoot, "binance.json"|]);
    let bytes = Node.Fs.readFileSync(binanceKeysFilename, `utf8);
    bytes |> Json.parseOrRaise |> Decode.apiKeys;
  };
};

[@bs.val] [@bs.scope ("process", "mainModule")]
external mainModulePaths : array(string) = "paths";

let projectRoot =
  switch (
    mainModulePaths[0] |> Js.String.split("/node_modules") |> Js.Array.shift
  ) {
  | Some(string) => string
  | None => raise(Not_found)
  };