let index: Index.t = [|
  {symbol: "CC1", priceUsd: 50.00, marketCapUsd: 5000.00}, /* 50% */
  {symbol: "CC2", priceUsd: 20.00, marketCapUsd: 2000.00}, /* 20% */
  {symbol: "CC3", priceUsd: 15.00, marketCapUsd: 1500.00}, /* 15% */
  {symbol: "CC4", priceUsd: 10.00, marketCapUsd: 1000.00}, /* 10% */
  {symbol: "CC5", priceUsd: 5.00, marketCapUsd: 500.00} /*     5% */
  /*                                               sum: 10,000.00 */
|];

let portfolio: Portfolio.t = [|
  {symbol: "CC1", balance: 10.0}, /* 500.00 */
  {symbol: "CC2", balance: 20.0}, /* 400.00 */
  {symbol: "CC5", balance: 20.0} /*  100.00 */
|];

Tape.test("TradeList", t => {
  t.plan(1);
  t.test("fromIndexAndPortfolio", t2 => {
    t2.plan(5);
    let tradeList = TradeList.fromIndexAndPortfolio(index, portfolio);
    t2.equalFloat(tradeList[0].balanceDelta, 0.0);
    t2.equalFloat(tradeList[1].balanceDelta, -10.0);
    t2.equalFloat(tradeList[2].balanceDelta, 10.0);
    t2.equalFloat(tradeList[3].balanceDelta, 10.0);
    t2.equalFloat(tradeList[4].balanceDelta, -10.0);
  });
});