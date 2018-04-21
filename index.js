const fs = require('fs')
const path = require('path')

const parseArgs = require('minimist')
const {parse: json2csv} = require('json2csv')

const {getTopCryptoCoins, getBinanceAccountInfo} = require('./lib/js/src/Fetchers.bs')
const {projectRoot} = require('./lib/js/src/Config.bs')
const {Binance: [_, __, getApiKeys]} = require('./lib/js/src/Config.bs') // FIXME: yuck -- array?!

const NUM_COINS = 15
const TOP_COINS_FILENAME = path.resolve(projectRoot, 'tmp', 'top-coins.csv')
const BALANCES_FILENAME = path.resolve(projectRoot, 'tmp', 'binance-balances.csv')

const writeSnapshots = async () => {
  const [apiKey, secret] = getApiKeys(projectRoot) // FIXME: yuck -- array?!

  const topCoinData = await getTopCryptoCoins(NUM_COINS)
  const topCoinCsv = json2csv(topCoinData)
  fs.writeFileSync(TOP_COINS_FILENAME, topCoinCsv)

  const {balances} = await getBinanceAccountInfo(apiKey, secret)
  const nonZeroBalances = balances.filter(({asset, free}) => parseFloat(free) > 0)
  const balancesCsv = json2csv(nonZeroBalances)
  fs.writeFileSync(BALANCES_FILENAME, balancesCsv)
}

const run = argv => {
  return new Promise((resolve, reject) => {
    const args = parseArgs(argv.slice(2), {
      alias: {h: 'help'},
      boolean: ['h'],
    })


    if (args.h) {
      console.info(`
Usage: ${argv[1]} [options]

Options:
  -h | --help: print this help message
`)
      return resolve()
    }

    return writeSnapshots()
  })
}

if (!module.parent) {
  run(process.argv)
    .then(() => {
      process.exit(0)
    })
    .catch(err => {
      console.error(`Failure: ${err}`)
    })
}