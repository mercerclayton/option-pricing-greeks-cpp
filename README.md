# Option Pricing and Greeks Library (C++)

An object-oriented C++17 library for pricing European and perpetual American options under the generalized Black–Scholes model, with closed-form Greeks, put–call parity checks, and finite-difference Greeks validated against the exact values.

Built as part of QuantNet's *C++ Programming for Financial Engineering* course.

## Features

- **One model, four underlyings.** A single cost-of-carry parameter `b` switches between models:

  | `b` | Model |
  |---|---|
  | `r` | Black–Scholes (non-dividend stock) |
  | `r - q` | Merton (continuous dividend yield `q`) |
  | `0` | Black (futures options) |
  | `r - R` | Garman–Kohlhagen (FX, foreign rate `R`) |

- **European options**: exact call and put prices, plus Delta, Gamma, Vega and Theta.
- **Put–call parity**: price one side from the other, and check whether a call and put pair is consistent with parity.
- **Perpetual American options**: closed-form call and put prices, with the early-exercise boundary handled and a clear error when no finite price exists.
- **Grid pricing**: generic templates apply any price or Greek to a vector or matrix of parameter sets, for example a table of prices across expiry and spot.
- **Finite-difference Greeks**: central-difference Delta and Gamma, compared against the exact values across step sizes to show how truncation error trades off against round-off error.
- **Input validation**: invalid parameters (non-positive expiry, strike, volatility or spot, including NaN) are rejected when they are set.

## Project structure

```
include/
  Option.hpp                   Abstract base class (Price() is pure virtual)
  OptionData.hpp               OptionParameters struct, validation, VaryParameter
  EuropeanOption.hpp           European option: price, Greeks, parity
  PerpetualAmericanOption.hpp  Perpetual American option
  MatrixPricer.hpp             Compute/ParameterGrid templates, DeltaFD/GammaFD
  Mesh.hpp                     Evenly spaced mesh generator
src/
  Option.cpp
  EuropeanOption.cpp
  PerpetualAmericanOption.cpp
  Mesh.cpp
  main.cpp                     Test driver that prints every result
```

## Building

Requires a C++17 compiler, CMake 3.14 or later, and Boost (header-only, for `boost::math::normal_distribution`).

```bash
# Install Boost if needed
# macOS:          brew install boost
# Ubuntu/Debian:  sudo apt install libboost-dev
# Windows:        vcpkg install boost-math

cmake -S . -B build
cmake --build build
./build/option_pricer
```

## Example

```cpp
#include "EuropeanOption.hpp"

// T = 0.25, K = 65, sig = 30%, r = 8%, S = 60, b = r (stock option)
OptionParameters params{0.25, 65, 0.30, 0.08, 60, 0.08};

EuropeanOption call{params, OptionType::Call};
double price = call.Price();   // 2.133368
double delta = call.Delta();
double put = call.ParityPrice(); // 5.846282, the put price via put–call parity
```

## Sample output

The test driver checks every result against benchmark values:

```
Batch 1: (T=0.250000, K=65.000000, sig=0.300000, r=0.080000, S=60.000000, b=0.080000)
  Call: 2.133368  (Expected: 2.133370)
  Put:  5.846282  (Expected: 5.846280)
  Parity satisfied (direct prices):   true

Perpetual: (T=inf, K=100.000000, sig=0.100000, r=0.100000, S=110.000000, b=0.020000)
  Call: 18.503500  (Expected: 18.5035)
  Put:  3.031060  (Expected: 3.03106)
```

Finite-difference Delta and Gamma against the exact values (futures option, exact Delta 0.594629):

```
         h     Delta FD  Delta Error     Gamma FD  Gamma Error
     1e+00     0.594580     4.82e-05     0.013493     8.27e-07
     1e-02     0.594629     4.82e-09     0.013494     4.06e-11
     1e-04     0.594629     3.91e-11     0.013493     4.31e-07
     1e-07     0.594629     2.90e-08     1.421085     1.41e+00
```

The error falls as `h` shrinks, until round-off takes over. This happens much sooner for Gamma, because its formula divides by `h²`.

## References

- E. G. Haug, *The Complete Guide to Option Pricing Formulas*, 2nd ed. (generalized Black–Scholes and perpetual American formulas)
