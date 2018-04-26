[%raw "global.fetch = require('node-fetch')"];

module Api = {
  let baseUrl = "https://api.coinmarketcap.com/v1";
  let tickerUrl = (numCoins: int) => {j|$(baseUrl)/ticker/?limit=$(numCoins)|j};
  module Fetchers = {
    module Decode = {
      let coinDecode = json : Index.Coin.t =>
        Json.Decode.{
          symbol:
            Symbol.getCanonical(
              ~symbol=json |> field("symbol", string),
              ~host=Host.CoinMarketCap,
            ),
          priceUsd: json |> field("price_usd", string) |> Js.Float.fromString,
          marketCapUsd:
            json |> field("market_cap_usd", string) |> Js.Float.fromString,
        };
      let indexDecode = json => json |> Json.Decode.array(coinDecode);
    };
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
          |> then_(json => resolve(json |> Decode.indexDecode))
          |> then_(coins =>
               resolve(
                 Js.Array.map((coin: Index.Coin.t) => coin.symbol, coins),
               )
             )
        )
      );
    };
  };
};