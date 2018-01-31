//
//  example.cpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 07/11/16.
//

#include "gsl_rootfinder.hpp"

#include <vector>

/*
GSL example on:
 Find root of Rosenbrock system of equations
 */

class User {
public:
  struct user_parameters {
    double a;
    double b;
  };

  User() : p({1.0, 10.0}){};
  ~User(){};

  double function(double *x, double *f) {
    f[0] = p.a * (1 - x[0]);
    f[1] = p.b * (x[1] - x[0] * x[0]);
    return 0;
  }

  struct user_parameters p;
};

int main() {

  // User Function
  User user;

  // Set user function
  auto f = [&](double *X, double *F) { return user.function(X, F); };

  // Initial guess
  std::vector<double> x0 = {-10, -5};

  // Initialize root solver
  gsl_modules::RootMultiFinder root;

  root.find(f, x0);

  std::cout << "\nThe solution is: ";
  for (std::size_t i = 0; i < 2; ++i) {
    std::cout << x0[i] << "\t";
  }
  std::cout << std::endl;

  return 0;
}
