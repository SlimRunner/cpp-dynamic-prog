#pragma once

#include "EnumUtils.hpp"
#include "Iterators.hpp"
#include "MutMatrix.hpp"

#include <algorithm> // max
#include <cassert>   // pair
#include <cstddef>   // size_t
#include <deque>
#include <functional> // functional
#include <limits>     // numeric_limits
#include <stack>
#include <utility> // pair

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
  for (size_t j = 1; j < M; ++j) {
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

template <class Seq, class Elem, class Num>
std::vector<std::pair<Seq, Seq>>
findAlignmentsNW(const MutMatrix<Num> &btrace, const Seq &seq1, const Seq &seq2,
                 size_t branchLimit = 10) {
  static_assert(
      std::is_same<decltype(std::declval<Seq>().at(0)), Elem &>::value ||
          std::is_same<decltype(std::declval<Seq>().at(0)),
                       const Elem &>::value,
      "Container Seq must support 'at()' method returning Elem or const Elem "
      "reference");
  // constexpr const auto max_size_t = std::numeric_limits<size_t>::max();
  using elemDeque = std::deque<Elem>;
  struct Vec2 {
    size_t x;
    size_t y;
    size_t branch;
    char sym1;
    char sym2;
  };
  std::stack<Vec2> stack;
  std::vector<std::pair<elemDeque, elemDeque>> seqs;

  seqs.push_back({elemDeque{}, elemDeque{}});
  stack.push({btrace.rowSize(), btrace.colSize(), 0, '\0', '\0'});

  for (Vec2 here = stack.top(); !stack.empty();) {
    here = stack.top();
    stack.pop();
    Num direction;

    if (here.x != seq1.size() && here.y != seq2.size()) {
      seqs.at(here.branch).first.push_front(here.sym1);
      seqs.at(here.branch).second.push_front(here.sym2);
    }
    if (here.x != 0 && here.y != 0) {
      direction = btrace(here.x - 1, here.y - 1);
    } else if (here.x != here.y) {
      direction = static_cast<Num>(here.x == 0 ? Dir8::WEST : Dir8::NORTH);
    } else {
      // branch has terminated
      continue;
    }

    bool branching = false;
    for (auto const &dir : {Dir8::WEST, Dir8::NORTH, Dir8::NORTH_WEST}) {
      using namespace enutl;
      if (direction & static_cast<int>(dir)) {

        if (branching && seqs.size() >= branchLimit) {
          break;
        }

        char c1 = dir & Dir8::WEST ? '-' : seq1.at(here.x - 1);
        char c2 = dir & Dir8::NORTH ? '-' : seq2.at(here.y - 1);
        size_t xNext = here.x - (dir & Dir8::WEST ? 0 : 1);
        size_t yNext = here.y - (dir & Dir8::NORTH ? 0 : 1);
        size_t nextBranch = here.branch;
        if (branching) {
          auto prev1 = seqs.at(here.branch).first;
          auto prev2 = seqs.at(here.branch).second;
          seqs.push_back({elemDeque{prev1}, elemDeque{prev2}});
          nextBranch = seqs.size() - 1;
        }
        stack.push({xNext, yNext, nextBranch, c1, c2});
        branching = true;
      }
    }
  }

  std::vector<std::pair<Seq, Seq>> out;
  for (auto const &pair : seqs) {
    out.push_back({Seq(pair.first.begin(), pair.first.end()),
                   Seq(pair.second.begin(), pair.second.end())});
  }

  return out;
}

} // namespace alg
