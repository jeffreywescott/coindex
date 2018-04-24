[%raw "global.fetch = require('node-fetch')"];

module Api = {
  let baseUrl = "https://api.binance.com/api/v3";
  module Credentials = {
    type t = {
      apiKey: string,
      secret: string,
    };
    let decode = json =>
      Json.Decode.{
        apiKey: json |> field("apiKey", string),
        secret: json |> field("secret", string),
      };
    let load = projectRoot => {
      let binanceKeysFilename =
        Node.Path.join([|projectRoot, "binance.json"|]);
      let bytes = Node.Fs.readFileSync(binanceKeysFilename, `utf8);
      bytes |> Json.parseOrRaise |> decode;
    };
  };
  module Sign = {
    type hmac;
    [@bs.module "crypto"] external createHmac : (string, string) => 'hmac = "";
    [@bs.send] external update : (hmac, string) => unit = "";
    [@bs.send] external digest : (hmac, Js.Nullable.t(string)) => string = "";
    let signHmacSha256 = (message: string, secret: string) => {
      let signed = createHmac("sha256", secret);
      update(signed, message);
      digest(signed, Js.Nullable.return("hex"));
    };
  };
  module Fetchers = {
    let getAccountInfo = (apiKey, secret) => {
      let now = Js.Date.now();
      let qs = {j|timestamp=$(now)|j};
      let signature = Sign.signHmacSha256(qs, secret);
      let accountUrl = baseUrl ++ {j|/account?$(qs)&signature=$(signature)|j};
      Js.Promise.(
        Fetch.(
          fetchWithInit(
            accountUrl,
            RequestInit.make(
              ~headers=
                HeadersInit.make({
                  "Accept": "application/json",
                  "X-MBX-APIKEY": apiKey,
                }),
              (),
            ),
          )
          |> then_(ApiUtils.responseJsonOrError)
        )
      );
    };
  };
};

module Account = {
  module Decode = {
    let assetDecode = json : Asset.t =>
      Json.Decode.{
        symbol: json |> field("asset", string),
        balance: json |> field("free", string) |> Js.Float.fromString,
      };
    let balancesDecode = json =>
      Json.Decode.(json |> field("balances", array(assetDecode)));
  };
  module Encode = {
    let assetEncode = (r: Asset.t) => {
      let canonicalSymbol =
        Asset.getCanonicalSymbol(~symbol=r.symbol, ~host=Host.Binance);
      Json.Encode.object_([
        ("symbol", Json.Encode.string(canonicalSymbol)),
        ("balance", Json.Encode.float(r.balance)),
      ]);
    };
    let balancesEncode = balances =>
      balances |> Json.Encode.array(assetEncode);
  };
};

let balancesFilename =
  Node.Path.join([|Config.projectRoot, "tmp", "binance-balances.csv"|]);