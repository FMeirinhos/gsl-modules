//
//  example.cpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 19/12/16.
//

#include "gsl_interpolator.hpp"

#include <cmath>
#include <iostream>
#include <vector>

using Vector = std::vector<double>;

// Create equiditant grid (copy of numpy.linspace)
template <typename T>
T linspace(double start, double stop, std::size_t n, bool endpoint = true) {
  T vec(n);

  double div;
  (endpoint) ? div = n - 1 : div = n;

  const double dx = (stop - start) / div;

  double _start = start;
  for (std::size_t i = 0; i < n; ++i) {
    vec[i] = _start;
    _start += dx;
  }

  return vec;
}

// Example on GSL Interpolation
int main() {

  const size_t size = 1 << 10;

  // Parameters
  const double bound = M_PI;

  Vector x = linspace<decltype(x)>(0, bound, size);
  Vector y(size);

  for (std::size_t i = 0; i < size; ++i)
    y[i] = sin(x[i]);

  gsl_modules::Interpolator<Vector, Vector> interp(x, y);

  const size_t new_size = size << 2;
  Vector new_x = linspace<decltype(x)>(0, bound, new_size);
  Vector new_y(new_size);

  interp.interpolate(new_x, new_y);

  std::cout << new_y[size << 1] << std::endl;

  return 0;
}
