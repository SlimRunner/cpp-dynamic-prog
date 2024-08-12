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

template <class Seq, class Elem, class Num>
void sequenceScoringNW(const Seq &seq1, const Seq &seq2, MutMatrix<Num> &scores,
                       MutMatrix<Num> &btrace,
                       std::function<Num(Elem, Elem)> matchPenalty,
                       std::function<Num()> gapPenalty) {
  static_assert(
      std::is_same<decltype(std::declval<Seq>().at(0)), Elem &>::value ||
          std::is_same<decltype(std::declval<Seq>().at(0)),
                       const Elem &>::value,
      "Container Seq must support 'at()' method returning Elem or const Elem "
      "reference");
  const size_t N = scores.rowSize();
  const size_t M = scores.colSize();
  Num left, above, diag, maxScore;

  for (size_t i = 1; i < N; ++i) {
    scores(i, 0) = -i;
  }
  for (size_t j = 1; j < N; ++j) {
    scores(0, j) = -j;
  }

  for (size_t i = 1; i < N; ++i) {
    for (size_t j = 1; j < M; ++j) {
      left = scores(i, j - 1) + gapPenalty();
      above = scores(i - 1, j) + gapPenalty();
      diag =
          scores(i - 1, j - 1) + matchPenalty(seq1.at(i - 1), seq2.at(j - 1));
      maxScore = std::max({left, above, diag});
      scores(i, j) = maxScore;
      btrace(i - 1, j - 1) |=
          static_cast<Num>(left == maxScore ? Dir8::WEST : Dir8::NONE);
      btrace(i - 1, j - 1) |=
          static_cast<Num>(above == maxScore ? Dir8::NORTH : Dir8::NONE);
      btrace(i - 1, j - 1) |=
          static_cast<Num>(diag == maxScore ? Dir8::NORTH_WEST : Dir8::NONE);
    }
  }
}

} // namespace alg
