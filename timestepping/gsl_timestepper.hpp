//
//  runge_kutta.h
//  gsl-modules
//
//  Created by Francisco Meirinhos on 27/01/17.
//

#ifndef runge_kutta_h
#define runge_kutta_h

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

#include "../function.hpp"

#include <assert.h>
#include <iostream>
#include <memory>

namespace gsl_modules {
enum StepperType {
  RK23,   // Runge-Kutta (2, 3) method
  RK4,    // Explicit 4th order (classical) Runge-Kutta
  RKF45,  // Runge-Kutta-Fehlberg (4, 5)
  RKCK45, // Explicit embedded Runge-Kutta Cash-Karp (4, 5)
  RK89    // Explicit embedded Runge-Kutta Prince-Dormand (8, 9)
};

// Summon Time Stepper for Runge Love
class TimeStepper {

  // Smart Pointer Deleter
  class ODEDeleter {
  public:
    void operator()(gsl_odeiv2_driver *x) { gsl_odeiv2_driver_free(x); }
  };

  // Default Parameters
  struct StepperParams {
    double hstart = 1e-6;
    double epsabs = 1e-6;
    double epsrel = 1e-12;
  };

public:
  // Ctor
  TimeStepper(){};

  // Dtor
  ~TimeStepper(){};

  // Initializer
  template <typename Fn> void init(Fn &fn, size_t dimension, StepperType type) {

    _sys.set_function(fn, dimension);

    gsl_odeiv2_step_type *T;

    switch (type) {
    case StepperType::RK23:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rk2);
      break;
    case StepperType::RK4:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rk4);
      break;
    case StepperType::RKF45:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rkf45);
      break;
    case StepperType::RKCK45:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rkck);
      break;
    case StepperType::RK89:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rk8pd);
      break;
    default:
      std::cout << "ERROR: Type not yet defined";
      break;
    }

    _driver.reset(gsl_odeiv2_driver_alloc_y_new(_sys.get(), T, params.hstart,
                                                params.epsabs, params.epsrel));
  }

  // Step from t0 to t1 (updating t0 to t1)
  int step(double &t0, double t1, double *data) {
    return gsl_odeiv2_driver_apply(_driver.get(), &t0, t1, data);
  }

public:
  // Parameters for Time Stepping
  struct StepperParams params;

private:
  // https://www.gnu.org/software/gsl/manual/html_node/Driver.html#Driver
  std::unique_ptr<gsl_odeiv2_driver, ODEDeleter> _driver;

  // ODE system
  GSLODESystem _sys;
};

} // namespace gsl_modules
#endif /* runge_kutta_h */
