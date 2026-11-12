/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods
*/
#ifndef OPTION_HPP
#define OPTION_HPP

#include "OptionData.hpp"

/*
Representation of an Option as an abstract base class.
*/
class Option {

  protected:
    OptionParameters m_params; // Parameters required to price the option
    OptionType m_type;         // Call or Put

  public:
    /*
    Constructor which requires option pricing parameters.
    */
    Option(const OptionParameters& params, OptionType type);

    /*
    Destructor.
    */
    virtual ~Option() = default;

    /*
    Prices the option.
    */
    virtual double Price() const = 0;
};

#endif
