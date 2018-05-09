Tape.test("Api", t => {
  t.plan(3);
  t.test("Credentials.decode", t2 => {
    t2.plan(2);
    let json =
      Json.parseOrRaise(
        "{\"apiKey\": \"some-key\",\"secret\": \"some-secret\"}",
      );
    let decoded = Binance.Api.Credentials.decode(json);
    t2.equalStr(decoded.apiKey, "some-key", ~message="keys match");
    t2.equalStr(decoded.secret, "some-secret", ~message="secrets match");
  });
  t.test("Sign.signHmacSha256", t2 => {
    t2.plan(1);
    let signed = Binance.Api.Sign.signHmacSha256("message", "secret");
    t2.equalStr(
      signed,
      "8b5f48702995c1598c573db1e21866a9b825d4a794d169d7060a03605796360b",
    );
  });
  t.test("Fetchers.Decode", t2 => {
    t2.plan(2);
    t2.test("assetDecode", t3 => {
      t3.plan(2);
      let json =
        Json.parseOrRaise("{\"asset\": \"BTC\", \"free\": \"1.23\"}");
      let decoded = Binance.Api.Fetchers.Decode.assetDecode(json);
      t3.equalStr(decoded.symbol, "BTC", ~message="symbols match");
      t3.equalFloat(decoded.balance, 1.23, ~message="balances match");
    });
    t2.test("portfolioDecode", t3 => {
      t3.plan(2);
      let json =
        Json.parseOrRaise(
          {|
            {
              "balances": [{
                "asset": "BTC", "free": "1.23"
              }, {
                "asset": "ETH", "free": "4.56"
              }]
            }
      |},
        );
      let decoded = Binance.Api.Fetchers.Decode.portfolioDecode(json);
      t3.equalInt(Js.Array.length(decoded), 2, ~message="has 2 assets");
      t3.equalFloat(decoded[1].balance, 4.56, ~message="balances match");
      t3.endTest();
    });
  });
});