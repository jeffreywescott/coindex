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
        symbol: json |> field("symbol", string),
        marketCap:
          json |> field("market_cap_usd", string) |> Js.Float.fromString,
      };
    let indexDecode = json => Json.Decode.(json |> array(coinDecode));
  };
  module Encode = {
    let coinEncode = (r: Index.Coin.t) =>
      Json.Encode.object_([
        ("symbol", Json.Encode.string(r.symbol)),
        ("marketCap", Json.Encode.float(r.marketCap)),
      ]);
    let indexEncode = coins => coins |> Json.Encode.array(coinEncode);
  };
};

let topCoinsFilename =
  Node.Path.join([|Config.projectRoot, "tmp", "top-coins.csv"|]);