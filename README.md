# ledgercpp

## Setup
On Linux extra libraries might be needed:
```
libusb-1.0-0-dev libhidapi-dev
```

## Build
```shell
mkdir build
cd build
cmake ..
make -j8
```

## References
- [NEAR app on Ledger Nano](https://github.com/LedgerHQ/app-near)
- [NEAR transaction crafting and broadcasting example](https://github.com/near-examples/transaction-examples/blob/master/send-tokens-deconstructed.js)
- [ObEngine plugin that use this library](https://github.com/nbleuzen-ledger/ObEngine-near-ledger)
