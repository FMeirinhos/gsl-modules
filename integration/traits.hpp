//
//  traits.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 13/01/17.
//

#ifndef traits_hpp
#define traits_hpp

#include <chrono>
#include <tuple>

/*
Stolen from
http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

Traits of lambda functions
 */

namespace util {

template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)> {
  enum { arity = sizeof...(Args) };
  // arity is the number of arguments.

  typedef ReturnType result_type;

  template <size_t i> struct arg {
    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    // the i-th argument is equivalent to the i-th tuple element of a tuple
    // composed of those arguments.
  };
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
  enum { arity = sizeof...(Args) };
  // arity is the number of arguments.

  typedef ReturnType result_type;

  template <size_t i> struct arg {
    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    // the i-th argument is equivalent to the i-th tuple element of a tuple
    // composed of those arguments.
  };

  // See: https://stackoverflow.com/questions/36612596/tuple-to-parameter-pack
  using argument_t = std::tuple<Args...>;
};

template <size_t I, typename T> struct tuple_n {
  template <typename... Args>
  using type = typename tuple_n<I - 1, T>::template type<T, Args...>;
};

template <typename T> struct tuple_n<0, T> {
  template <typename... Args> using type = std::tuple<Args...>;
};
template <size_t I, typename T>
using tuple_of = typename tuple_n<I, T>::template type<>;

} // namespace util
#endif /* traits_hpp */
