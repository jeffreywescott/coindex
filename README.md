# coindex

A cryptocurrency index for the masses.

## What is this?

### MVP

- use the [CoinMarketCap API][coinmarketcap-api] to figure out the relative market cap of the top N coins (e.g., N = 15)
- use the [binance API][binance-api] to rebalance the user's portfolio based on relative market cap

## Getting Started

1. Clone the this repo: `git clone git@github.com:jeffreywescott/coindex.git`
2. Create your `binance.json` file using [`binance.json.sample`](binance.json.sample)
3. Run the `rebalance` script: `yarn run rebalance`

## Known Issues

- right now, all it does is save CSV files of the top coins and the non-zero portfolio balances
- name mismatches between CoinMarketCap and binance aren't handled appropriately

<!-- references -->

[coinmarketcap-api]: https://coinmarketcap.com/api/
[binance-api]: https://github.com/binance-exchange/binance-official-api-docs
