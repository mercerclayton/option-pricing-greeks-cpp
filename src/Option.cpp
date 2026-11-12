/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods

Implementation details of the Option class.
*/
#include "Option.hpp"

/*
Constructor which requires option pricing parameters.
*/
Option::Option(const OptionParameters& params, OptionType type)
    : m_params{params}, m_type{type} {}