//
//  rungekutta.cpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 27/01/17.
//

#include "gsl_timestepper.hpp"

#include <iostream>
#include <vector>

using namespace gsl_modules;

/*
Consider the Van der Pol oscillator equation,
    u''(t) + \mu u'(t) (u(t)^2 - 1) + u(t) = 0

This can be converted into a first order system suitable for use with the
routines described in this chapter by introducing a separate variable for the
velocity, v = u'(t),
    u' = v
    v' = -u + \mu v (1-u^2)
*/

class VanDerPol {
public:
  // Van Der Pol equation
  auto equation(const double *y, double *f) {
    f[0] = y[1];
    f[1] = -y[0] + 10 * y[1] * (1 - y[0] * y[0]);
  };

  // Parameters
  double mu = 10;

  // Initial State
  std::vector<double> y = {1.0, 0.0};

  // Dimension of the problem
  const size_t dimension = 2;
};

int main() {

  // Create System
  VanDerPol vdp;

  // Cast main equation into a lambda (this needs to be the function signature
  // for GSL!)
  auto vdp_eq = [&](double t, const double *y, double *f) {
    (void)t;
    return vdp.equation(y, f);
  };

  // Create time stepper
  TimeStepper ts;
  ts.init(vdp_eq, vdp.dimension, RK89);

  // Set time variables
  const double initialTime = 0.0, finalTime = 100.0;
  const size_t n_steps = 100;

  // Integrate!!
  double currentTime = 0;
  for (size_t i = 1; i <= n_steps; i++) {
    double nextTime = i * (finalTime - initialTime) / n_steps;
    int status = ts.step(currentTime, nextTime, vdp.y);

    if (status != GSL_SUCCESS) {
      printf("error, return value=%d\n", status);
      break;
    }

    // NOTE: It's very bad design to mix IOs like stdcout with computation, but
    // this is just an example
    printf("t =  %.2e \t u(t) = %.5e \t v(t) = %.5e\n", currentTime, vdp.y[0],
           vdp.y[1]);
  }

  return 0;
}
