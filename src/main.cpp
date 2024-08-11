#include "MutMatrix.hpp"
#include <array>
#include <vector>
#include <iostream>

int main(int, char const *[]) {
  std::vector<std::vector<int>> arr = {{1,2,3},{1,2,6}};

  alg::MutMatrix<int>mtx(arr.cbegin(), arr.cend());
  // const alg::MutMatrix<int>mtx(5, 7);
  std::cout << "(" << mtx.rowSize() << ", " << mtx.colSize() << ")";
  std::cout << std::endl;
  mtx(0,0) = 47;

  for (size_t i = 0, sz = mtx.rowSize(); i < sz; ++i) {
    for (auto && e : mtx.viewRow(i)) {
      std::cout << e << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  return 0;
}
