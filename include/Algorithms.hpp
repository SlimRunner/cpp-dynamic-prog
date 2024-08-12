#pragma once

namespace alg {

enum class Dir8 {
  NONE = 0,
  CENTER = 1 << 0,
  EAST = 1 << 1,
  NORTH_WEST = 1 << 2,
  NORTH = 1 << 3,
  NORTH_EAST = 1 << 4,
  SOUTH_WEST = 1 << 5,
  SOUTH = 1 << 6,
  SOUTH_EAST = 1 << 7,
  WEST = 1 << 8,
};

struct NeighborMark {
  using IterInt = PseudoIter<int>;
  int mark = static_cast<int>(Dir8::NONE);

  int operator&(const Dir8 &rhs) { return mark & static_cast<int>(rhs); }

  NeighborMark &operator|=(const Dir8 &rhs) {
    mark |= static_cast<int>(rhs);
    return *this;
  }

  IterInt begin() { return IterInt(0); }
  IterInt end() { return IterInt(9); }
};

} // namespace alg
