//
//  gsl_interpolator.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 09/01/17.
//

#ifndef gsl_interpolator_h
#define gsl_interpolator_h

#include <gsl/gsl_spline.h>

#include <cassert>
#include <cmath>
#include <vector>

namespace gsl_modules {

template <typename T1, typename T2> class Interpolator {
public:
  Interpolator(){};

  Interpolator(T1 &x, T2 &y) : _x(x.data()), _y(y.data()) {
    static_assert(std::is_same<T1, T2>::value, "Incompatible type");
    _size = x.size();
    interp = gsl_interp_alloc(gsl_interp_steffen, _size);
    acc = gsl_interp_accel_alloc();
    gsl_interp_init(interp, _x, _y, _size);
  }

  Interpolator(double *x, double *y, std::size_t size) : _x(x), _y(y) {
    _size = size;
    interp = gsl_interp_alloc(gsl_interp_steffen, _size);
    acc = gsl_interp_accel_alloc();
    gsl_interp_init(interp, _x, _y, _size);
  }

  void initialize(T1 &x, T2 &y) {
    _x = x.data();
    _y = y.data();
    _size = x.size();
    interp = gsl_interp_alloc(gsl_interp_cspline, _size);
    acc = gsl_interp_accel_alloc();
    gsl_interp_init(interp, _x, _y, _size);
  }

  ~Interpolator() {
    gsl_interp_accel_free(acc);
    gsl_interp_free(interp);
  }

  // Get value
  double operator()(double x) {
    return gsl_interp_eval(interp, _x, _y, x, acc);
  }

  // Interpolate over an array of x and y
  template <typename T3, typename T4> void interpolate(const T3 &x, T4 &y) {
    static_assert(std::is_same<T3, T4>::value, "Incompatible type");
    const size_t size = x.size();
    assert(size == y.size());

    // To avoid extrapolation due to loss of precision
    (fabs(_x[0] - x[0]) <= 1e-12) ? y[0] = operator()(_x[0])
                                  : y[0] = operator()(x[0]);

    for (size_t i = 1; i < size - 1; ++i)
      y[i] = operator()(x[i]);

    // To avoid extrapolation due to loss of precision
    (fabs(_x[_size - 1] - x[size - 1]) < 1e-12)
        ? y[size - 1] = operator()(_x[_size - 1])
        : y[size - 1] = operator()(x[size - 1]);
  }

  // Interpolate y over an array x
  void interpolate(const double *x, double *y, const std::size_t size) {
    // To avoid extrapolation due to loss of precision
    (fabs(_x[0] - x[0]) <= 1e-12) ? y[0] = operator()(_x[0])
                                  : y[0] = operator()(x[0]);

    for (int i = 1; i < size - 1; ++i)
      y[i] = operator()(x[i]);

    // To avoid extrapolation due to loss of precision
    (fabs(_x[_size - 1] - x[size - 1]) < 1e-12)
        ? y[size - 1] = operator()(_x[_size - 1])
        : y[size - 1] = operator()(x[size - 1]);
  }

private:
  // Pointers to Containers' Data
  double *_x;
  double *_y;

private:
  // GSL Objects
  gsl_interp *interp;
  gsl_interp_accel *acc;

  // Size of function
  std::size_t _size;
};
} // namespace gsl_modules
#endif /* gsl_interpolator_h */
