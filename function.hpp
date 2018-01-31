//
//  function.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 15/11/17.
//

#ifndef function_hpp
#define function_hpp

#include "gsl/gsl_integration.h"

namespace gsl_modules {

/*
Wraps a lambda function to a GSL function
*/

class GSLFunction {
public:
  // Set function to 0
  GSLFunction() {
    _f.function = nullptr;
    _f.params = 0;
  }

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
} // namespace gsl_modules
#endif /* function_hpp */
