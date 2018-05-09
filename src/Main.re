[@bs.module "json2csv"] external json2csv : Js.Json.t => string = "parse";

let writeTradeList = () =>
  Js.Promise.(
    TradeList.fetchIndexAndPortfolio()
    |> then_(((index, portfolio)) => {
         let tradeList = TradeList.fromIndexAndPortfolio(index, portfolio);
         resolve(tradeList |> TradeList.jsonEncode);
       })
    |> then_(json => resolve(json2csv(json)))
    |> then_(csv =>
         resolve(Node.Fs.writeFileSync(TradeList.filename, csv, `utf8))
       )
    |> ignore
  );

writeTradeList();