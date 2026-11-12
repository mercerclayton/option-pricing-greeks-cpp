/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods
*/
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "EuropeanOption.hpp"
#include "MatrixPricer.hpp"
#include "Mesh.hpp"
#include "OptionData.hpp"
#include "PerpetualAmericanOption.hpp"

/*
A parameter set together with the call and put prices given in the PDF.
*/
struct TestCase {
    OptionParameters params;
    double expectedCall;
    double expectedPut;
};

/*
Prints a matrix with row labels down the left and column labels across the
top. corner is printed above the row labels, e.g. "T \ S".
*/
void PrintMatrix(const std::string& corner,
                 const std::vector<double>& rowLabels,
                 const std::vector<double>& colLabels, const Matrix& matrix,
                 int rowLabelPrecision = 2, int colLabelPrecision = 0) {

    const auto oldPrecision = std::cout.precision();

    std::cout << std::setw(8) << corner << std::setprecision(colLabelPrecision);
    for (double c : colLabels) {
        std::cout << std::setw(12) << c;
    }
    std::cout << '\n';

    for (std::size_t i = 0; i < matrix.size(); ++i) {
        std::cout << std::setw(8) << std::setprecision(rowLabelPrecision)
                  << rowLabels[i] << std::setprecision(oldPrecision);
        for (double value : matrix[i]) {
            std::cout << std::setw(12) << value;
        }
        std::cout << '\n';
    }

    std::cout << std::setprecision(oldPrecision);
}

int main() {

    std::cout << std::fixed << std::setprecision(6);

    // Calculations that can be passed to Compute
    const auto callPrice = [](const OptionParameters& p) {
        return EuropeanOption{p, OptionType::Call}.Price();
    };

    const auto putPrice = [](const OptionParameters& p) {
        return EuropeanOption{p, OptionType::Put}.Price();
    };

    const auto callDelta = [](const OptionParameters& p) {
        return EuropeanOption{p, OptionType::Call}.Delta();
    };

    const auto gamma = [](const OptionParameters& p) {
        return EuropeanOption{p, OptionType::Call}
            .Gamma(); // same for calls and puts
    };

    const auto perpCall = [](const OptionParameters& p) {
        return PerpetualAmericanOption{p, OptionType::Call}.Price();
    };

    const auto perpPut = [](const OptionParameters& p) {
        return PerpetualAmericanOption{p, OptionType::Put}.Price();
    };

    // ------------------------------------------------------------------------
    std::cout << "=== Mesh Function ===\n" << std::setprecision(1);

    const auto mesh = Mesh(10, 50, 1);
    std::cout << "Mesh(10, 50, 1):  " << mesh.size() << " points, first "
              << mesh.front() << ", last " << mesh.back() << "\n";

    try {
        Mesh(0, 1, 0);
    } catch (const std::invalid_argument& e) {
        std::cout << "Mesh(0, 1, 0) threw as expected: " << e.what() << '\n';
    }

    std::cout << std::setprecision(6);

    // ------------------------------------------------------------------------
    // Batches 1-4 (stock options, so b = r)
    std::vector<TestCase> batches{
        {{0.25, 65, 0.30, 0.08, 60, 0.08}, 2.13337, 5.84628},
        {{1.0, 100, 0.20, 0.0, 100, 0.0}, 7.96557, 7.96557},
        {{1.0, 10, 0.50, 0.12, 5, 0.12}, 0.204058, 4.07326},
        {{30.0, 100, 0.30, 0.08, 100, 0.08}, 92.17570, 1.24750},
    };

    std::cout << "\n=== European Call and Put Prices, Put-Call Parity ===";
    for (std::size_t i = 0; i < batches.size(); ++i) {
        const TestCase& tc = batches[i];
        const double call = callPrice(tc.params);
        const double put = putPrice(tc.params);

        std::cout << "\nBatch " << i + 1 << ": " << tc.params << '\n'
                  << "  Call: " << call << "  (Expected: " << tc.expectedCall
                  << ")\n"
                  << "  Put:  " << put << "  (Expected: " << tc.expectedPut
                  << ")\n";

        const double putFromParity =
            EuropeanOption{tc.params, OptionType::Call}.ParityPrice();
        const double callFromParity =
            EuropeanOption{tc.params, OptionType::Put}.ParityPrice();

        std::cout << "  Put via parity:  " << putFromParity << '\n'
                  << "  Call via parity: " << callFromParity << '\n'
                  << "  Parity satisfied (direct prices):   " << std::boolalpha
                  << EuropeanOption::SatisfiesParity(call, put, tc.params)
                  << '\n'
                  << "  Parity satisfied (expected prices): "
                  << EuropeanOption::SatisfiesParity(
                         tc.expectedCall, tc.expectedPut, tc.params, 1e-4)
                  << '\n';
    }

    // ------------------------------------------------------------------------
    std::cout << "\n=== Prices for S = 30, 31, ..., 70 (Batch 1) ===\n";
    const auto sValues = Mesh(30, 70, 1);
    const auto sParams =
        VaryParameter(batches[0].params, &OptionParameters::S, sValues);

    const auto callPrices = Compute(sParams, callPrice);
    const auto putPrices = Compute(sParams, putPrice);

    std::cout << "     S         Call          Put\n";
    for (std::size_t i = 0; i < sValues.size(); ++i) {
        std::cout << std::setw(6) << std::setprecision(0) << sValues[i]
                  << std::setprecision(6) << std::setw(13) << callPrices[i]
                  << std::setw(13) << putPrices[i] << '\n';
    }

    try {
        VaryParameter(batches[0].params, &OptionParameters::sig, {0.2, -0.1});
    } catch (const std::invalid_argument& e) {
        std::cout << "Negative volatility rejected as expected: " << e.what()
                  << '\n';
    }

    // ------------------------------------------------------------------------
    const auto sGrid = Mesh(55, 75, 5);

    std::cout << "\n=== Call Prices: Rows = T, Columns = S (Batch "
                 "1) ===\n";
    const std::vector<double> tValues{0.25, 0.5, 1.0, 2.0};
    const Matrix callMatrix =
        Compute(ParameterGrid(batches[0].params, &OptionParameters::T, tValues,
                              &OptionParameters::S, sGrid),
                callPrice);
    PrintMatrix("T \\ S", tValues, sGrid, callMatrix);

    std::cout << "\n=== Put Prices: Rows = sig, Columns = S (Batch "
                 "1) ===\n";
    const auto sigValues = Mesh(0.1, 0.5, 0.1);
    const Matrix putMatrix =
        Compute(ParameterGrid(batches[0].params, &OptionParameters::sig,
                              sigValues, &OptionParameters::S, sGrid),
                putPrice);
    PrintMatrix("sig \\ S", sigValues, sGrid, putMatrix);

    // ------------------------------------------------------------------------
    // Futures option data set (b = 0)
    const OptionParameters futures{0.5, 100, 0.36, 0.1, 105, 0.0};
    const EuropeanOption futCall{futures, OptionType::Call};
    const EuropeanOption futPut{futures, OptionType::Put};

    std::cout << "\n=== Exact Greeks (Futures Option) ===\n";
    std::cout << "Futures: " << futures << '\n'
              << "  Call Delta: " << futCall.Delta() << "  (Expected: 0.5946)\n"
              << "  Put Delta:  " << futPut.Delta() << "  (Expected: -0.3566)\n"
              << "  Call Gamma: " << futCall.Gamma() << '\n'
              << "  Put Gamma:  " << futPut.Gamma() << '\n'
              << "  Call Vega:  " << futCall.Vega() << '\n'
              << "  Put Vega:   " << futPut.Vega() << '\n'
              << "  Call Theta: " << futCall.Theta() << '\n'
              << "  Put Theta:  " << futPut.Theta() << '\n';

    // ------------------------------------------------------------------------
    std::cout
        << "\n=== Call Delta for S = 75, 80, ..., 135 (Futures Option) ===\n";
    const auto deltaS = Mesh(75, 135, 5);
    const auto callDeltas = Compute(
        VaryParameter(futures, &OptionParameters::S, deltaS), callDelta);

    std::cout << "     S   Call Delta\n";
    for (std::size_t i = 0; i < deltaS.size(); ++i) {
        std::cout << std::setw(6) << std::setprecision(0) << deltaS[i]
                  << std::setprecision(6) << std::setw(13) << callDeltas[i]
                  << '\n';
    }

    // ------------------------------------------------------------------------
    const auto greekS = Mesh(85, 125, 10);

    std::cout
        << "\n=== Call Delta: Rows = T, Columns = S (Futures Option) ===\n";
    const std::vector<double> greekT{0.25, 0.5, 1.0, 2.0};
    const Matrix deltaMatrix =
        Compute(ParameterGrid(futures, &OptionParameters::T, greekT,
                              &OptionParameters::S, greekS),
                callDelta);
    PrintMatrix("T \\ S", greekT, greekS, deltaMatrix);

    std::cout << "\n=== Gamma: Rows = sig, Columns = S (Futures Option) ===\n";
    const auto greekSig = Mesh(0.1, 0.5, 0.1);
    const Matrix gammaMatrix =
        Compute(ParameterGrid(futures, &OptionParameters::sig, greekSig,
                              &OptionParameters::S, greekS),
                gamma);
    PrintMatrix("sig \\ S", greekSig, greekS, gammaMatrix);

    // ------------------------------------------------------------------------
    std::cout
        << "\n=== Divided Differences vs Exact Greeks (Futures Option) ===\n";
    std::cout << "Exact Call Delta: " << futCall.Delta()
              << "   Exact Gamma: " << futCall.Gamma() << '\n';
    std::cout
        << "         h     Delta FD  Delta Error     Gamma FD  Gamma Error\n";
    for (double h : {1.0, 0.1, 0.01, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7}) {
        const double deltaFD = DeltaFD(callPrice, futures, h);
        const double gammaFD = GammaFD(callPrice, futures, h);
        std::cout << std::scientific << std::setprecision(0) << std::setw(10)
                  << h << std::fixed << std::setprecision(6) << std::setw(13)
                  << deltaFD << std::scientific << std::setprecision(2)
                  << std::setw(13) << std::abs(deltaFD - futCall.Delta())
                  << std::fixed << std::setprecision(6) << std::setw(13)
                  << gammaFD << std::scientific << std::setprecision(2)
                  << std::setw(13) << std::abs(gammaFD - futCall.Gamma())
                  << '\n';
    }
    std::cout << std::fixed << std::setprecision(6);

    std::cout
        << "\n=== Call Delta, Exact vs Divided Difference (h = 0.01) ===\n";
    const auto fdDeltas = Compute(
        VaryParameter(futures, &OptionParameters::S, deltaS),
        [&](const OptionParameters& p) { return DeltaFD(callPrice, p, 0.01); });

    std::cout << "     S        Exact           FD        Error\n";
    for (std::size_t i = 0; i < deltaS.size(); ++i) {
        std::cout << std::setw(6) << std::setprecision(0) << deltaS[i]
                  << std::setprecision(6) << std::setw(13) << callDeltas[i]
                  << std::setw(13) << fdDeltas[i] << std::scientific
                  << std::setprecision(2) << std::setw(13)
                  << std::abs(fdDeltas[i] - callDeltas[i]) << std::fixed
                  << std::setprecision(6) << '\n';
    }

    // ------------------------------------------------------------------------
    // Perpetual American data set (no expiry, so T = infinity)
    const double noExpiry = std::numeric_limits<double>::infinity();
    const OptionParameters perpetual{noExpiry, 100, 0.1, 0.1, 110, 0.02};

    std::cout << "\n=== Perpetual American Call and Put Prices ===\n";
    std::cout << "Perpetual: " << perpetual << '\n'
              << "  Call: "
              << PerpetualAmericanOption{perpetual, OptionType::Call}.Price()
              << "  (Expected: 18.5035)\n"
              << "  Put:  "
              << PerpetualAmericanOption{perpetual, OptionType::Put}.Price()
              << "  (Expected: 3.03106)\n";

    try {
        PerpetualAmericanOption{perpetual.With(&OptionParameters::b, 0.1),
                                OptionType::Call}
            .Price();
    } catch (const std::domain_error& e) {
        std::cout << "Call with b = r rejected as expected: " << e.what()
                  << '\n';
    }

    // ------------------------------------------------------------------------
    std::cout
        << "\n=== Perpetual American Prices for S = 60, 65, ..., 160 ===\n";
    const auto perpS = Mesh(60, 160, 5);
    const auto perpParams =
        VaryParameter(perpetual, &OptionParameters::S, perpS);

    const auto perpCalls = Compute(perpParams, perpCall);
    const auto perpPuts = Compute(perpParams, perpPut);

    std::cout << "     S         Call          Put\n";
    for (std::size_t i = 0; i < perpS.size(); ++i) {
        std::cout << std::setw(6) << std::setprecision(0) << perpS[i]
                  << std::setprecision(6) << std::setw(13) << perpCalls[i]
                  << std::setw(13) << perpPuts[i] << '\n';
    }

    // ------------------------------------------------------------------------
    const auto perpGridS = Mesh(80, 140, 15);

    std::cout << "\n=== Perpetual Call Prices: Rows = sig, Columns = S ===\n";
    const auto perpSig = Mesh(0.1, 0.3, 0.05);
    const Matrix perpCallMatrix =
        Compute(ParameterGrid(perpetual, &OptionParameters::sig, perpSig,
                              &OptionParameters::S, perpGridS),
                perpCall);
    PrintMatrix("sig \\ S", perpSig, perpGridS, perpCallMatrix);

    std::cout << "\n=== Perpetual Put Prices: Rows = r, Columns = S ===\n";
    const std::vector<double> perpR{0.05, 0.10, 0.15, 0.20};
    const Matrix perpPutMatrix =
        Compute(ParameterGrid(perpetual, &OptionParameters::r, perpR,
                              &OptionParameters::S, perpGridS),
                perpPut);
    PrintMatrix("r \\ S", perpR, perpGridS, perpPutMatrix);

    return 0;
}
