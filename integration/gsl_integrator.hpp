//
//  gsl_integrator.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 13/01/17.
//

#ifndef gsl_integrator_hpp
#define gsl_integrator_hpp

#include "../function.hpp"
#include "gsl/gsl_integration.h"

#include <memory>
#include <type_traits>
#include <utility>

namespace gsl_modules {

namespace detail {

struct IntegratorParams {
  double epsabs = 1e-8; // Absolute error
  double epsrel = 1e-3; // Relative error
  size_t n_eval = 1e3;  // Maximum number of evaluations
  size_t limit = 1e2;   // Maximum number of function pieces
  int key = 4; // 1,2,3,4,5&6 corresponding to 15,21,31,41,51 & 61 point
               // Gauss-Kronrod rules
};

/*
Base of all integrators. An example of a curiously recurring template pattern
*/
template <typename _Integrator> class IntegratorBase {
public:
  using boundary_t = std::pair<double, double>;

  /// Default Ctor
  IntegratorBase(std::size_t workspace_size = 1e3) {
    workspace_ = gsl_integration_workspace_alloc(workspace_size);
  }

  /// Default Dtor
  virtual ~IntegratorBase() { gsl_integration_workspace_free(workspace_); }

  /// Set user function
  template <typename Fn> void set_function(Fn &fn) { F_.set_function(fn); }

  void set_params(double epsabs, double epsrel, std::size_t n_eval = 1e3,
                  std::size_t limit = 1e2, int key = 1) {
    p_.epsabs = epsabs;
    p_.epsrel = epsrel;
    p_.n_eval = n_eval;
    p_.limit = limit;
    p_.key = key;
  }

protected:
  // GSL Workspace
  gsl_integration_workspace *workspace_;

  // Wrapped GSL Function
  GSLFunction F_;

  // Parameters
  struct detail::IntegratorParams p_;
};

} // namespace detail

/*
Integrator using QAG
https://www.gnu.org/software/gsl/manual/html_node/QAG-adaptive-integration.html#QAG-adaptive-integration
*/

class IntegratorQag : public detail::IntegratorBase<IntegratorQag> {
public:
  template <typename Fn, typename Boundaries>
  double integrate(Fn &fn, Boundaries &&boundaries) {

    this->F_.set_function(fn);

    return integrate(std::forward<Boundaries>(boundaries));
  }

  template <typename Boundaries>
  inline double integrate(Boundaries &&boundaries) {

    double result, error;
    gsl_integration_qag(this->F_.get(), boundaries.first, boundaries.second,
                        this->p_.epsabs, this->p_.epsrel, this->p_.limit,
                        this->p_.key, this->workspace_, &result, &error);
    return result;
  }
};

/*
Integrator using QNG
https://www.gnu.org/software/gsl/manual/html_node/QNG-non_002dadaptive-Gauss_002dKronrod-integration.html#QNG-non_002dadaptive-Gauss_002dKronrod-integration
*/
class IntegratorQng : public detail::IntegratorBase<IntegratorQng> {
public:
  template <typename Fn, typename Boundaries>
  double integrate(Fn &fn, Boundaries &&boundaries) {

    this->F_.set_function(fn);

    return integrate(std::forward<Boundaries>(boundaries));
  }

  template <typename Boundaries>
  inline double integrate(Boundaries &&boundaries) {

    double result, error;
    size_t neval;
    gsl_integration_qng(this->F_.get(), boundaries.first, boundaries.second,
                        this->p_.epsabs, this->p_.epsrel, &result, &error,
                        &neval);
    return result;
  }
};

/*
Integrator using CQUAD
https://www.gnu.org/software/gsl/manual/html_node/CQUAD-doubly_002dadaptive-integration.html#CQUAD-doubly_002dadaptive-integration
*/
class IntegratorQuad {
public:
  using boundary_t = std::pair<double, double>;

  /// Default Ctor
  IntegratorQuad(std::size_t workspace_size = 1e2) {
    workspace_ = gsl_integration_cquad_workspace_alloc(workspace_size);
  }

  /// Default Dtor
  ~IntegratorQuad() { gsl_integration_cquad_workspace_free(workspace_); }

  template <typename Fn> void set_function(Fn &fn) { F_.set_function(fn); }

  template <typename Fn, typename Boundaries>
  double integrate(Fn &fn, Boundaries &&boundaries) {

    this->F_.set_function(fn);

    return integrate(std::forward<Boundaries>(boundaries));
  }

  template <typename Boundaries>
  inline double integrate(Boundaries &&boundaries) {

    static_assert(std::is_same<typename std::remove_reference<Boundaries>::type,
                               boundary_t>::value,
                  "Boundaries not of the type IntegratorQuad::boundary_t");

    double result, error;
    size_t neval;
    gsl_integration_cquad(this->F_.get(), boundaries.first, boundaries.second,
                          this->p_.epsabs, this->p_.epsrel, this->workspace_,
                          &result, &error, &neval);
    return result;
  }

  void set_params(double epsabs, double epsrel) {
    p_.epsabs = epsabs;
    p_.epsrel = epsrel;
  }

private:
  // GSL Workspace
  gsl_integration_cquad_workspace *workspace_;

  // Wrapped GSL Function
  GSLFunction F_;

  // Parameters
  struct detail::IntegratorParams p_;
};

} // namespace gsl_modules

#endif /* gsl_integrator_hpp */
