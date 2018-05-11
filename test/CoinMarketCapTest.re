Tape.test("Api", t => {
  t.plan(1);
  t.test("Fetchers.Decode", t2 => {
    t2.plan(2);
    t2.test("coinDecode", t3 => {
      t3.plan(3);
      let json =
        Json.parseOrRaise(
          {| {"symbol": "BTC", "price_usd": "12345.67", "market_cap_usd": "1234567890"} |},
        );
      let decoded = CoinMarketCap.Api.Fetchers.Decode.coinDecode(json);
      t3.equalStr(decoded.symbol, "BTC", ~message="symbols match");
      t3.equalFloat(decoded.priceUsd, 12345.67, ~message="prices match");
      t3.equalFloat(
        decoded.marketCapUsd,
        1234567890.0,
        ~message="market caps match",
      );
    });
    t2.test("indexDecode", t3 => {
      t3.plan(3);
      let json =
        Json.parseOrRaise(
          {|
            [{
              "symbol": "BTC", "price_usd": "12345.67", "market_cap_usd": "1234567890"
            }, {
              "symbol": "ETH", "price_usd": "678.90", "market_cap_usd": "987654321"
            }]
      |},
        );
      let decoded = CoinMarketCap.Api.Fetchers.Decode.indexDecode(json);
      t3.equalInt(Js.Array.length(decoded), 2, ~message="has 2 coins");
      t3.equalFloat(decoded[0].priceUsd, 12345.67, ~message="prices match");
      t3.equalFloat(
        decoded[1].marketCapUsd,
        987654321.0,
        ~message="market caps match",
      );
      t3.endTest();
    });
  });
});