/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods
*/
#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

/*
Returns the evenly spaced points start, start + h, start + 2h, ..., up to and
including end (if end lies on the mesh). Throws std::invalid_argument if
h <= 0 or end < start.
Example: Mesh(10, 50, 1) -> {10, 11, ..., 50}
*/
std::vector<double> Mesh(double start, double end, double h);

#endif
