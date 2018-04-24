[%raw "global.fetch = require('node-fetch')"];

module Api = {
  let baseUrl = "https://api.coinmarketcap.com/v1";
  let tickerUrl = (numCoins: int) => {j|$(baseUrl)/ticker/?limit=$(numCoins)|j};
  module Fetchers = {
    let getTopCryptoCoins = numCoins => {
      let url = tickerUrl(numCoins);
      Js.Promise.(
        Fetch.(
          fetchWithInit(
            url,
            RequestInit.make(
              ~headers=HeadersInit.make({"Accept": "application/json"}),
              (),
            ),
          )
          |> then_(ApiUtils.responseJsonOrError)
        )
      );
    };
  };
};

module TopCoinsIndex = {
  let numCoins = 15;
  module Decode = {
    let coinDecode = json : Index.Coin.t =>
      Json.Decode.{
        symbol:
          Symbol.getCanonical(
            ~symbol=json |> field("symbol", string),
            ~host=Host.Binance,
          ),
        priceUsd: json |> field("price_usd", string) |> Js.Float.fromString,
        marketCapUsd:
          json |> field("market_cap_usd", string) |> Js.Float.fromString,
      };
    let indexDecode = json => Json.Decode.(json |> array(coinDecode));
  };
};

let topCoinsFilename =
  Node.Path.join([|Config.projectRoot, "tmp", "top-coins.csv"|]);