Tape.test("Symbol", t => {
  t.plan(1);
  t.test("getCanonical", t2 => {
    t2.plan(2);
    t2.test("returns the canonical name if there is one", t3 => {
      t3.plan(2);
      t3.ok(Symbol.getCanonical(~symbol="BCC", ~host=Host.Binance) == "BCH");
      t3.ok(
        Symbol.getCanonical(~symbol="MIOTA", ~host=Host.CoinMarketCap)
        == "IOT",
      );
    });
    t2.test("returns the passed-in symbol if no canonical name is found", t3 => {
      t3.plan(1);
      t3.ok(
        Symbol.getCanonical(
          ~symbol="NOTAREALSYMBOL",
          ~host=Host.CoinMarketCap,
        )
        == "NOTAREALSYMBOL",
      );
    });
  });
});