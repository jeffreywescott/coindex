Tape.test("CryptoCompare.Api", t => {
  let coinJson = {|{
    "TYPE": "5",
    "MARKET": "CCCAGG",
    "FROMSYMBOL": "ETH",
    "TOSYMBOL": "USD",
    "FLAGS": "4",
    "PRICE": 671.63,
    "LASTUPDATE": 1526071860,
    "LASTVOLUME": 1.95618319,
    "LASTVOLUMETO": 1313.1270899513,
    "LASTTRADEID": "243402740",
    "VOLUMEDAY": 683849.0610957603,
    "VOLUMEDAYTO": 475265891.84783727,
    "VOLUME24HOUR": 728048.2946468813,
    "VOLUME24HOURTO": 507644453.10426956,
    "OPENDAY": 723.63,
    "HIGHDAY": 735.97,
    "LOWDAY": 666.79,
    "OPEN24HOUR": 737.55,
    "HIGH24HOUR": 740.58,
    "LOW24HOUR": 666.48,
    "LASTMARKET": "Bitfinex",
    "CHANGE24HOUR": -65.91999999999996,
    "CHANGEPCT24HOUR": -8.937699139041417,
    "CHANGEDAY": -52,
    "CHANGEPCTDAY": -7.185992841645593,
    "SUPPLY": 99375137.6553,
    "MKTCAP": 66743323703.429146,
    "TOTALVOLUME24H": 2230422.4917068747,
    "TOTALVOLUME24HTO": 1516684035.0756729
  }|};
  let rawJson = {j| {"ETH": {"USD": $(coinJson)}} |j};
  let coinsJson = {j| {"RAW": $(rawJson)} |j};
  let assertFields = (t: Tape.testFuncs, decoded: Index.Coin.t) => {
    t.equalStr(decoded.symbol, "ETH", ~message="symbols match");
    t.equalFloat(decoded.priceUsd, 671.63, ~message="prices match");
    t.equalFloat(
      decoded.marketCapUsd,
      66743323703.429146,
      ~message="market caps match",
    );
  };
  t.plan(1);
  t.test("Fetchers.Decode", t2 => {
    t2.plan(3);
    t2.test("coinDecode", t3 => {
      t3.plan(3);
      let json = Json.parseOrRaise(coinJson);
      let decoded = CryptoCompare.Api.Fetchers.Decode.coinDecode(json);
      assertFields(t3, decoded);
    });
    t2.test("rawDecode", t3 => {
      t3.plan(3);
      let json = Json.parseOrRaise(rawJson);
      let decoded = CryptoCompare.Api.Fetchers.Decode.rawDecode("ETH", json);
      assertFields(t3, decoded);
    });
    t2.test("coinsDecode", t3 => {
      t3.plan(3);
      let json = Json.parseOrRaise(coinsJson);
      let decoded =
        CryptoCompare.Api.Fetchers.Decode.coinsDecode([|"ETH"|], json);
      assertFields(t3, decoded[0]);
    });
  });
});