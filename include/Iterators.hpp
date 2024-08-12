#pragma once

#include <type_traits> // is_integral

namespace alg {

template <class Elem> class PseudoIter {
  static_assert(std::is_integral<Elem>::value, "Integral required.");

private:
  Elem iter;

public:
  PseudoIter(Elem i) : iter{i} {}

  Elem operator*() const { return iter; }

  PseudoIter &operator++() {
    iter++;
    return *this;
  }

  bool operator==(const PseudoIter &other) const { return iter == other.iter; }
  bool operator!=(const PseudoIter &other) const { return iter != other.iter; }
};

} // namespace alg
