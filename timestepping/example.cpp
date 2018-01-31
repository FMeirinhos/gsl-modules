//
//  rungekutta.cpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 27/01/17.
//

#include "gsl_timestepper.hpp"
#include <vector>

// Your RHS should inherit from a ODESystem
class Equation : public gsl_modules::ODESystem {
public:
  // Ctor
  Equation() {
    // DO NOT FORGET TO DECLARE THE DIMENSION
    dim = 2;
  }

  // Dtor
  ~Equation() {}

  // Right Hand Side NEEDS TO BE (DOUBLE, CONST DOUBLE*, DOUBLE*)
  int RHS(double t, const double *y, double *f) {
    f[0] = y[1];
    f[1] = -y[0] - mu * y[1] * (y[0] * y[0] - 1);
    return GSL_SUCCESS;
    (void)t; // to avoid unused errors
  }

private:
  // User parameters
  double mu = 10;
};

int main() {

  Equation sys;

  gsl_modules::TimeStepper ts;
  ts.init(sys, gsl_modules::RK89);

  const double initialTime = 0.0, finalTime = 100.0;
  const int n_steps = 100;

  // Initial State
  std::vector<double> y = {1.0, 0.0};

  double currentTime = 0;
  for (int i = 1; i <= n_steps; i++) {
    double ti = i * (finalTime - initialTime) / n_steps;
    int status = ts.step(currentTime, ti, y.data());

    if (status != GSL_SUCCESS) {
      printf("error, return value=%d\n", status);
      break;
    }

    printf("%.5e %.5e %.5e\n", currentTime, y[0], y[1]);
  }

  return 0;
}
