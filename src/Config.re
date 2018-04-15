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
  let getApiKeys = () => {
    let dirname: option(string) = [%bs.node __dirname];
    let dir =
      switch (dirname) {
      | Some(dirname) => dirname
      | None => "."
      };
    let binanceKeysFilename = Node.Path.join([|dir, "..", "binance.json"|]);
    let bytes = Node.Fs.readFileSync(binanceKeysFilename, `utf8);
    bytes |> Json.parseOrRaise |> Decode.apiKeys;
  };
};