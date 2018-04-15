[%raw "global.fetch = require('node-fetch')"];

exception APIError(string);

open Fetch;

open Sign;

let responseOrError = resp =>
  Response.ok(resp) ?
    Response.json(resp) :
    raise(
      APIError(
        string_of_int(Response.status(resp)) ++ Response.statusText(resp),
      ),
    );

let getTopCryptoCoins = numCoins => {
  let url = Config.CoinMarketCap.tickerUrl(numCoins);
  Js.Promise.(
    fetchWithInit(
      url,
      RequestInit.make(
        ~headers=HeadersInit.make({"Accept": "application/json"}),
        (),
      ),
    )
    |> then_(responseOrError)
  );
};

let getBinanceAccountInfo = (apiKey, secret) => {
  let now = Js.Date.now();
  let qs = {j|timestamp=$(now)|j};
  let signature = signHmacSha256(qs, secret);
  let url =
    Config.Binance.baseUrl ++ {j|/account?$(qs)&signature=$(signature)|j};
  Js.Promise.(
    fetchWithInit(
      url,
      RequestInit.make(
        ~headers=
          HeadersInit.make({
            "Accept": "application/json",
            "X-MBX-APIKEY": apiKey,
          }),
        (),
      ),
    )
    |> then_(responseOrError)
  );
};