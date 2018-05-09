[%raw "global.fetch = require('node-fetch')"];

module Api = {
  let baseUrl = "https://min-api.cryptocompare.com/data";
  module Fetchers = {
    module Decode = {
      let coinDecode = json : Index.Coin.t => {
        symbol:
          Symbol.getCanonical(
            ~symbol=
              json |> Json.Decode.field("FROMSYMBOL", Json.Decode.string),
            ~host=Host.CryptoCompare,
          ),
        priceUsd: json |> Json.Decode.field("PRICE", Json.Decode.float),
        marketCapUsd: json |> Json.Decode.field("MKTCAP", Json.Decode.float),
      };
      let usdDecode = json => json |> Json.Decode.field("USD", coinDecode);
      let rawDecode = (coin, json) =>
        json |> Json.Decode.field(coin, usdDecode);
      let coinsDecode = (symbols: array(Symbol.t), json: Js.Json.t) =>
        Js.Array.map(
          symbol => json |> Json.Decode.field("RAW", rawDecode(symbol)),
          symbols,
        );
    };
    let getIndex = (symbols: array(Symbol.t)) => {
      let csCoins = Js.Array.joinWith(",", symbols);
      let priceMultiFullUrl = {j|$(baseUrl)/pricemultifull?fsyms=$(csCoins)&tsyms=USD|j};
      Js.Promise.(
        Fetch.(
          fetchWithInit(
            priceMultiFullUrl,
            RequestInit.make(
              ~headers=HeadersInit.make({"Accept": "application/json"}),
              (),
            ),
          )
          |> then_(ApiUtils.responseJsonOrError)
          |> then_(json => resolve(json |> Decode.coinsDecode(symbols)))
        )
      );
    };
  };
};