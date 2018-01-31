//
//  multi_root.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 07/11/16.
//

#ifndef multi_root_hpp
#define multi_root_hpp

#include <gsl/gsl_multiroots.h>

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

namespace gsl_modules {

namespace detail {

class GSLMultirootFunction {
public:
  // Default ctor
  GSLMultirootFunction() {
    _f.f = 0;
    _f.params = nullptr;
    _f.n = 0;
  }

  // Set function with parameters
  template <typename _Lambda>
  void set_function(_Lambda &lambda, size_t func_size) {
    _f.f = GSLMultirootFunction::functor<_Lambda>;
    _f.params = reinterpret_cast<void *>(&lambda);
    _f.n = func_size;
  }

  // Get the gsl_multiroot_function
  gsl_multiroot_function *get() { return &_f; }

private:
  template <typename _Lambda>
  static int functor(const gsl_vector *x, void *p, gsl_vector *f) {
    _Lambda *_func = reinterpret_cast<_Lambda *>(p);
    (*_func)(x->data, f->data);
    return GSL_SUCCESS;
  }

private:
  // The wrapped function!
  gsl_multiroot_function _f;
};

// Cast normal vector to GSL
template <typename T> class gsl_vector_cast : public gsl_vector {
public:
  gsl_vector_cast(T &user_vec) : _vec(user_vec) {
    // TODO CHECK FOR CONTAINER TYPE
    // static_assert(std::is_same<double, double>::value, "not compatible");

    data = reinterpret_cast<double *>(_vec.data());
    size = _vec.size();
    stride = 1;
    owner = 1;
    // block->data = *_vec;
    // block->size = _vec.size();
  }

private:
  T &_vec;
};

struct RootMultiFinderParams {
  double epsabs = 1e-8;
  double epsrel = 1e-3;
  int maxit = 35;
  size_t size = 0;
};
} // namespace detail

class RootMultiFinder {

public:
  // Solver type
  enum class SolverType { dnewton, broyden, hybrid, hybrids };

  // Default Ctor
  RootMultiFinder() {}

  // Default Dtor
  ~RootMultiFinder() { gsl_multiroot_fsolver_free(rsolver); }

  // Find root (Python like)
  template <typename Fn, typename T>
  bool find(Fn &fn, T &guess, SolverType st = SolverType::hybrids,
            bool monitor_ = true) {
    const std::size_t dim = guess.size();

    set_solver(st, dim);
    monitor = monitor_;

    set_guess(guess);
    set_function(fn);

    return find(x);
  }

  // Set function
  template <typename T> void set_function(T &user_func) {
    assert(_p.size != 0 && "Function size needs to be defined before");
    F.set_function(user_func, _p.size);
  }

  // Set guess
  template <typename T> void set_guess(T &user_vector) {
    x0 = detail::gsl_vector_cast<T>(user_vector);
    x.resize(user_vector.size());
  }

  // Find root
  template <typename T> bool find(T &result) {

    // static_assert(std::is_same<T, double>::value, "not compatible type");

    // Set GSL solver
    gsl_multiroot_fsolver_set(rsolver, F.get(), &x0);

    // Reset variables
    iter = 0;
    status = 0;

    if (monitor) {
      std::cout << std::endl
                << std::endl
                << "************* Now iterating *************" << std::endl;
      print_state(iter, rsolver);
    }

    // Root Finding Routine
    do {
      iter++;
      status = gsl_multiroot_fsolver_iterate(rsolver);
      if (monitor)
        print_state(iter, rsolver);

      if (status)
        break;

      status = gsl_multiroot_test_residual(rsolver->f, _p.epsabs);
    } while (status == GSL_CONTINUE && iter < _p.maxit);

    memcpy(result.data(), rsolver->x->data, sizeof(double) * rsolver->x->size);

    return status;
  };

  // Set parameters
  void set_params(double epsabs, double epsrel, int maxit) {
    _p.epsabs = epsabs;
    _p.epsrel = epsrel;
    _p.maxit = maxit;
  }

  // Set type of solver
  void set_solver(SolverType type, size_t n_dimensions) {
    switch (type) {
    case SolverType::dnewton:
      rsolver = gsl_multiroot_fsolver_alloc(gsl_multiroot_fsolver_dnewton,
                                            n_dimensions);
      break;
    case SolverType::broyden:
      rsolver = gsl_multiroot_fsolver_alloc(gsl_multiroot_fsolver_broyden,
                                            n_dimensions);
      break;
    case SolverType::hybrid:
      rsolver = gsl_multiroot_fsolver_alloc(gsl_multiroot_fsolver_hybrid,
                                            n_dimensions);
      break;
    case SolverType::hybrids:
      rsolver = gsl_multiroot_fsolver_alloc(gsl_multiroot_fsolver_hybrids,
                                            n_dimensions);
      break;
    default:
      std::cout << "Root solver type not defined";
      throw;
    }
    _p.size = n_dimensions;
  }

public:
  bool monitor = true;
  std::vector<double> x;

private:
  void print_state(size_t iter, gsl_multiroot_fsolver *s) {
    std::cout.precision(4);
    std::cout << '\r' << "iter = " << iter << "\t\t x = ";
    for (size_t i = 0; i < _p.size; ++i) {
      std::cout << std::setw(10) << s->x->data[i] << "\t";
    }
    std::cout << "\t\tf(x) = ";
    for (size_t i = 0; i < _p.size; ++i) {
      std::cout << std::setw(10) << s->f->data[i] << "\t";
    }
    std::cout << std::flush;
  }

private:
  // Solver without derivative
  gsl_multiroot_fsolver *rsolver;

  // GSL Function
  detail::GSLMultirootFunction F;

  // Parameters
  struct detail::RootMultiFinderParams _p;

  // Gsl vector for guessing
  gsl_vector x0;

  // Variables for convergence
  int iter;
  int status;
};

} // namespace gsl_modules
#endif /* multi_root_hpp */
