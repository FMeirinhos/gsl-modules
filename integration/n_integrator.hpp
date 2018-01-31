//
//  n_integrator.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 19/04/2017.
//

#ifndef n_integrator_hpp
#define n_integrator_hpp

#include "gsl_integrator.hpp"
#include "traits.hpp"
#include "tuple_at.hpp"

#include <array>
#include <tuple>
#include <utility>

/*
 N-dimensional integrator.
 Recursively wraps integrators inside integrators for an n-dimensional
 integrator.
 */

namespace gsl_modules {

template <std::size_t Dimension, typename _Integrator = IntegratorQuad>
class Integrator {
public:
  using integrator_t = _Integrator;
  using integrators_t = util::tuple_of<Dimension, _Integrator>;
  using boundary_t =
      util::tuple_of<Dimension, typename _Integrator::boundary_t>;

  /// Ctor
  Integrator() {}

  /// Dtor
  ~Integrator() {}

  /// Get integral
  template <typename Fn> double integrate(Fn &&fn, boundary_t boundaries) {

    // NOTE: Cannot return type and arity of all kinds of user functions since
    // it's not possible to determine the arity of generic lambdas

    //     static_assert(
    // std::is_same<double, typename
    // util::function_traits<Fn>::result_type>::value,
    //     "User function does not return real numbers");
    //
    // static_assert(util::function_traits<Fn>::arity == Dimension,
    //               "User function arguments does not match dimensions");

    return integrator<1, Fn>{}(integrators_, std::forward<Fn>(fn), boundaries);
  }

private:
  /// general form integrator
  template <std::size_t, typename...> struct integrator {};

  /// base integrator
  template <typename Fn, typename... Args>
  struct integrator<Dimension, Fn, Args...> {
    double operator()(integrators_t &integrators, Fn &&fn,
                      boundary_t &boundaries, Args &&... args) {
      auto integrand = [&](double x) {
        return std::forward<Fn>(fn)(std::forward<Args>(args)..., x);
      };
      return std::get<sizeof...(Args)>(integrators)
          .integrate(integrand, std::get<sizeof...(Args)>(boundaries));
    }
  };

  /// recursive integrator
  template <std::size_t D, typename Fn, typename... Args>
  struct integrator<D, Fn, Args...> {
    double operator()(integrators_t &integrators, Fn &&fn,
                      boundary_t &boundaries, Args &&... args) {
      auto integrand = [&](double x) {
        return integrator<D + 1, Fn, double, Args...>{}(
            integrators, std::forward<Fn>(fn), boundaries,
            std::forward<Args>(args)..., std::forward<double>(x));
      };
      return std::get<sizeof...(Args)>(integrators)
          .integrate(integrand, std::get<sizeof...(Args)>(boundaries));
    }
  };

private:
  integrators_t integrators_;

public:
  // TODO: iterate over loop properly
  // http://foonathan.net/blog/2017/03/01/tuple-iterator.html
  template <typename... Args> void set_params(Args... args) {
    for (std::size_t i = 0; i < Dimension; ++i) {
      auto set = [&](_Integrator &intg) { intg.set_params(args...); };
      util::visit_at(integrators_, i, set);
    }
  }
};

} // namespace gsl_modules
#endif /* n_integrator_hpp */
