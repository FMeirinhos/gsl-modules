//
//  function.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 15/11/17.
//

#ifndef function_hpp
#define function_hpp

#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_odeiv2.h>

namespace gsl_modules {

/*
Wraps a lambda function to a GSL function
*/

class GSLFunction {
public:
  /// Default ctor
  GSLFunction() : _f({nullptr, 0}) {}

  // Store the lambda in params of gsl_function
  template <typename Fn> void set_function(Fn &lambda) {
    _f.function = GSLFunction::functor<Fn>;
    _f.params = reinterpret_cast<void *>(&lambda);
  }

  // Overload operator for testing
  double operator()(double x) { return GSL_FN_EVAL(&_f, x); }

  // Get the wrapped function
  gsl_function *get() { return &_f; }

private:
  template <typename Fn> static double functor(double x, void *p) {
    Fn *function = reinterpret_cast<Fn *>(p);
    return (*function)(x);
  }

private:
  // The wrapped function!
  gsl_function _f;
};

/*
Wraps a lambda in a GSL ODE System
*/

class GSLODESystem {
public:
  /// Default ctor
  GSLODESystem() : _sys({nullptr, nullptr, 0, nullptr}) {}

  // Sets the ODE system
  template <typename Fn> void set_function(Fn &lambda, size_t dimension) {
    _sys.function = GSLODESystem::functor<Fn>;
    _sys.jacobian = nullptr;
    _sys.dimension = dimension;
    _sys.params = reinterpret_cast<void *>(&lambda);
  };

  // Get gsl_odeiv2_system
  gsl_odeiv2_system *get() { return &_sys; }

private:
  template <typename Fn>
  static int functor(double t, const double y[], double dydt[], void *params) {
    Fn *function = reinterpret_cast<Fn *>(params);
    (*function)(t, y, dydt);
    return GSL_SUCCESS;
  };
  //
  // This data type defines a general ODE system
  gsl_odeiv2_system _sys;
};

} // namespace gsl_modules
#endif /* function_hpp */
