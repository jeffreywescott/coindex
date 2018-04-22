[@bs.val] external __dirname : string = "";

let rec findProjectRoot = startDir : string => {
  let packageJson = Node.Path.join([|startDir, "package.json"|]);
  let nodeModules = Node.Path.join([|startDir, "node_modules"|]);
  Node.Fs.existsSync(packageJson) && Node.Fs.existsSync(nodeModules) ?
    startDir :
    {
      let parts = Js.String.split("/", startDir);
      let parentParts = Js.Array.slice(parts, ~start=0, ~end_=-1);
      findProjectRoot(Js.Array.joinWith("/", parentParts));
    };
};

let projectRoot = findProjectRoot(__dirname);

module CoinMarketCap = {
  let baseUrl = "https://api.coinmarketcap.com/v1";
  let tickerUrl = (numCoins: int) => {j|$(baseUrl)/ticker/?limit=$(numCoins)|j};
  let numCoins = 15;
  let topCoinsFilename =
    Node.Path.join([|projectRoot, "tmp", "top-coins.csv"|]);
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
  let balancesFilename =
    Node.Path.join([|projectRoot, "tmp", "binance-balances.csv"|]);
};