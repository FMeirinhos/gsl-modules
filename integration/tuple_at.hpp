//
//  tuple_at.hpp
//  gsl-modules
//
//  Created by Francisco Meirinhos on 13/01/17.
//

#ifndef tuple_at_hpp
#define tuple_at_hpp

#include <cassert>
#include <tuple>

// See http://foonathan.net/blog/2017/03/01/tuple-iterator.html for a better
// tuple iterator

namespace util {

// See: https://stackoverflow.com/questions/36612596/tuple-to-parameter-pack

template <size_t I, typename T> struct tuple_n {
  template <typename... Args>
  using type = typename tuple_n<I - 1, T>::template type<T, Args...>;
};

template <typename T> struct tuple_n<0, T> {
  template <typename... Args> using type = std::tuple<Args...>;
};
template <size_t I, typename T>
using tuple_of = typename tuple_n<I, T>::template type<>;

/*
Stolen from
https://stackoverflow.com/questions/28997271/c11-way-to-index-tuple-at-runtime-without-using-switch
*/
template <size_t I> struct visit_impl {
  template <typename T, typename F>
  static void visit(T &tup, size_t idx, F fun) {
    if (idx == I - 1)
      fun(std::get<I - 1>(tup));
    else
      visit_impl<I - 1>::visit(tup, idx, fun);
  }
};

template <> struct visit_impl<0> {
  template <typename T, typename F> static void visit(T &, size_t, F) {
    assert(false);
  }
};

template <typename F, typename... Ts>
void visit_at(std::tuple<Ts...> const &tup, size_t idx, F fun) {
  visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
}

template <typename F, typename... Ts>
void visit_at(std::tuple<Ts...> &tup, size_t idx, F fun) {
  visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
}

} // namespace util

#endif /* tuple_at_hpp */
