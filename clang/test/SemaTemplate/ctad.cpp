// RUN: %clang_cc1 -std=c++23 -verify %s

namespace pr41427 {
  template <typename T> class A {
  public:
    A(void (*)(T)) {}
  };

  void D(int) {}

  void f() {
    A a(&D);
    using T = decltype(a);
    using T = A<int>;
  }
}

namespace Access {
  struct B {
  protected:
    struct type {};
  };
  template<typename T> struct D : B { // expected-note {{not viable}}
    D(T, typename T::type); // expected-note {{private member}}
  };
  D b = {B(), {}};

  class X {
    using type = int;
  };
  D x = {X(), {}}; // expected-error {{no viable constructor or deduction guide}}

  // Once we implement proper support for dependent nested name specifiers in
  // friends, this should still work.
  class Y {
    template <typename T> friend D<T>::D(T, typename T::type); // expected-warning {{dependent nested name specifier}}
    struct type {};
  };
  D y = {Y(), {}};

  class Z {
    template <typename T> friend class D;
    struct type {};
  };
  D z = {Z(), {}};
}

namespace warn_ctad_copy {
#pragma clang diagnostic push
#pragma clang diagnostic warning "-Wctad-selects-copy"
// like std::revrse_iterator, the motivating example
template <typename T> struct CTAD {
  template <typename From>
    requires(!__is_same(From, T) && __is_convertible_to(From, T))
  CTAD(const CTAD<From>&) {} // #1
  CTAD(T) {}
};
CTAD(void) -> CTAD<void>; // does not suppress -Wctad-selects-copy

CTAD<int>&   fc1();
CTAD<double> fc2(),
             c1 = fc1();         // uses #1
CTAD c2 = fc2(),                 // OK, uses copy-initialization
     c3 = CTAD(4.2);             // OK, copy deduction candidate not selected
CTAD c4{fc2()};                  // test prvalue expression
// expected-warning@-1{{move-constructing not wrapping a 'CTAD<double>' \
(aka 'warn_ctad_copy::CTAD<double>'), use copy-list-initialization to suppress this warning}}
CTAD c5 = CTAD{fc1()};           // test lvalue  expression
// expected-warning@-1{{copy-constructing not wrapping a 'CTAD<int>' \
(aka 'warn_ctad_copy::CTAD<int>'), use copy-list-initialization to suppress this warning}}
auto c6 = CTAD((CTAD<int>&&)c5); // test xvalue  expression
// expected-warning@-1{{move-constructing not wrapping a 'CTAD<int>' \
(aka 'warn_ctad_copy::CTAD<int>'), use copy-initialization to suppress this warning}}
#pragma clang diagnostic pop
} // namespace warn_ctad_copy
