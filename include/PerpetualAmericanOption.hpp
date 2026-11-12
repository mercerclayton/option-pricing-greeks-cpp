/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods
*/
#ifndef PERPETUAL_AMERICAN_OPTION_HPP
#define PERPETUAL_AMERICAN_OPTION_HPP

#include "Option.hpp"

/*
Representation of a perpetual American option: it never expires and can be
exercised at any time. The expiry time T in the parameters is not used.
*/
class PerpetualAmericanOption : public Option {

  protected:
    /*
    Calculates y1, the exponent in the call formula.
    */
    double Y1() const;

    /*
    Calculates y2, the exponent in the put formula.
    */
    double Y2() const;

  public:
    /*
    Constructor which requires option pricing parameters.
    */
    PerpetualAmericanOption(const OptionParameters& params, OptionType type);

    /*
    Prices the option. Throws std::domain_error if no finite price exists
    (call with b >= r, put with r <= 0).
    */
    double Price() const override;
};

#endif
