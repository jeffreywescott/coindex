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

let numTopCoins = 15;

let topCoinsFilename =
  Node.Path.join([|Config.projectRoot, "tmp", "top-coins.csv"|]);