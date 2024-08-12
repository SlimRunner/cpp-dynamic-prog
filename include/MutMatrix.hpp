#pragma once

#include <algorithm> // copy
#include <cstddef>   // size_t
#include <exception> // invalid_argument
#include <iterator>  // back_inserter
#include <limits>    // numeric_limits
#include <type_traits>
#include <utility> // pair
#include <vector>

namespace alg {

// https://stackoverflow.com/a/30469265
template <class It> class RowView {
private:
  It mBegin;
  It mEnd;

public:
  RowView(It begin, It end) : mBegin(begin), mEnd(end) {}
  It begin() { return this->mBegin; }
  It end() { return this->mEnd; }
  It begin() const { return this->mBegin; }
  It end() const { return this->mEnd; }
};

template <class T> class MutMatrix {
private:
  std::vector<T> mData;
  size_t mRows;
  size_t mCols;

public:
  /* CONSTRUCTORS */

  MutMatrix() : mData(), mRows(0U), mCols(0U) {}

  MutMatrix(size_t rows, size_t columns)
      : mData(rows * columns), mRows(rows), mCols(columns) {}

  template <class It, typename = std::enable_if_t<!std::is_integral<It>::value>>
  MutMatrix(It begin, It end) : MutMatrix() {
    size_t validator = std::numeric_limits<size_t>::max();
    size_t prevSize = std::numeric_limits<size_t>::max();

    for (It it = begin; it != end; ++it) {
      std::copy(it->cbegin(), it->cend(), std::back_inserter(mData));
      ++mRows;

      if (validator == std::numeric_limits<size_t>::max()) {
        validator = mData.size();
        mCols = validator;
      } else if (validator != (mData.size() - prevSize)) {
        throw std::invalid_argument(
            "Array to construct matrix cannot be jagged");
      }
      prevSize = mData.size();
    }
  }

  /* OPERATORS */

  T &operator()(size_t row, size_t col) { return mData.at(col + row * mCols); }

  T operator()(size_t row, size_t col) const {
    return mData.at(col + row * mCols);
  }

  /* ITERATION */

  auto begin() { return mData.begin(); }

  auto end() { return mData.end(); }

  auto begin() const { return mData.begin(); }

  auto end() const { return mData.end(); }

  auto viewRow(size_t row) {
    auto begin = mData.begin() + static_cast<long>(row * mCols);
    auto end = begin + static_cast<long>(mCols);
    return RowView(begin, end);
  }

  auto viewRow(size_t row) const {
    auto begin = mData.begin() + static_cast<long>(row * mCols);
    auto end = begin + static_cast<long>(mCols);
    return RowView(begin, end);
  }

  /* PROPERTIES */

  size_t size() const { return mData.size(); }

  std::pair<size_t, size_t> sizePair() const {
    size_t r = mData.size();
    size_t c;

    if (!r) {
      c = 0U;
    } else {
      c = mData.back().size();
    }

    return {r, c};
  }

  size_t rowSize() const { return mRows; }

  size_t colSize() const { return mCols; }

  /* VALIDATORS */

  bool isInRange(size_t row, size_t col) {
    return 0 <= row && row < rowSize() && 0 <= col && colSize();
  }
};

} // namespace alg
