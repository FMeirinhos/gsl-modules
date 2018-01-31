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

#include <assert.h>
#include <iostream>

namespace gsl_modules {
enum StepperType {
  RK23,   // Runge-Kutta (2, 3) method
  RK4,    // Explicit 4th order (classical) Runge-Kutta
  RKF45,  // Runge-Kutta-Fehlberg (4, 5)
  RKCK45, // Explicit embedded Runge-Kutta Cash-Karp (4, 5)
  RK89    // Explicit embedded Runge-Kutta Prince-Dormand (8, 9)
};

// Parent Class for ODE Systems
class ODESystem {
public:
  // Ctor
  ODESystem() {}

  // Dtor
  virtual ~ODESystem() {}

  // Virtual bitch
  virtual int RHS(double t, const double *y, double *f) = 0;

  gsl_odeiv2_system *get() {
    // Set up ODE System
    if (!isSet) {
      void *params = (void *)this;
      _sys = {F, nullptr, dim, params};
    }

    return &_sys;
  }

private:
  // Fucking GSL Gymnastics
  static int F(double t, const double y[], double f[], void *params) {
    ODESystem *me = reinterpret_cast<ODESystem *>(params);
    return me->RHS(t, y, f);
  };

protected:
  // Dimension of the solution
  std::size_t dim;

private:
  // System to be evolved
  gsl_odeiv2_system _sys;

  // Is ODE Set up for GSL?
  bool isSet = false;
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
  void init(ODESystem &sys, StepperType type) {

    gsl_odeiv2_step_type *T;

    switch (type) {
    case RK23:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rk2);
      break;
    case RK4:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rk4);
      break;
    case RKF45:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rkf45);
      break;
    case RKCK45:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rkck);
      break;
    case RK89:
      T = const_cast<gsl_odeiv2_step_type *>(gsl_odeiv2_step_rk8pd);
      break;
    default:
      std::cout << "ERROR: Type not yet defined";
      break;
    }

    _driver.reset(gsl_odeiv2_driver_alloc_y_new(sys.get(), T, params.hstart,
                                                params.epsabs, params.epsrel));
  }

  // Step from t0 to t1 (updating t0 to t1)
  template <typename T> int step(double &t0, double t1, T *y) {
    return gsl_odeiv2_driver_apply(_driver.get(), &t0, t1, y);
  }

public:
  // Parameters for Time Stepping
  struct StepperParams params;

private:
  // https://www.gnu.org/software/gsl/manual/html_node/Driver.html#Driver
  std::unique_ptr<gsl_odeiv2_driver, ODEDeleter> _driver;
};
} // namespace gsl_modules
#endif /* runge_kutta_h */
