[%raw "global.fetch = require('node-fetch')"];

module Api = {
  let baseUrl = "https://api.coinmarketcap.com/v1";
  module Fetchers = {
    module Decode = {
      let coinDecode = json : Index.Coin.t =>
        Json.Decode.{
          symbol:
            Symbol.getCanonical(
              ~symbol=json |> field("symbol", string),
              ~host=Host.CoinMarketCap,
            ),
          priceUsd:
            switch (json |> optional(field("price_usd", string))) {
            | Some(s) => Js.Float.fromString(s)
            | None => 0.0
            },
          marketCapUsd:
            switch (json |> optional(field("market_cap_usd", string))) {
            | Some(s) => Js.Float.fromString(s)
            | None => 0.0
            },
        };
      let indexDecode = json => json |> Json.Decode.array(coinDecode);
    };
    let getAllCoins = () => {
      /* limit=0 will return _all_ coins */
      let url = {j|$(baseUrl)/ticker/?limit=0|j};
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
          |> then_(index => {
               /* remove lower market cap coins with duplicate symbols */
               let coinsByUniqueSymbol =
                 Js.Array.reduce(
                   (
                     coinsBySymbol: Js.Dict.t(Index.Coin.t),
                     coin: Index.Coin.t,
                   ) => {
                     let largerCoin =
                       switch (Js.Dict.get(coinsBySymbol, coin.symbol)) {
                       | Some(c) when c.marketCapUsd > coin.marketCapUsd => c
                       | Some(c) => c
                       | None => coin
                       };
                     Js.Dict.set(
                       coinsBySymbol,
                       largerCoin.symbol,
                       largerCoin,
                     );
                     coinsBySymbol;
                   },
                   Js.Dict.empty(),
                   index,
                 );
               let sortedIndex =
                 Js.Array.sortInPlaceWith(
                   (c1: Index.Coin.t, c2: Index.Coin.t) => {
                     let res = c2.marketCapUsd > c1.marketCapUsd ? 1 : (-1);
                     res;
                   },
                   Js.Dict.values(coinsByUniqueSymbol),
                 );
               resolve(sortedIndex);
             })
        )
      );
    };
  };
};