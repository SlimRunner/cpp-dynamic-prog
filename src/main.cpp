#include "Algorithms.hpp"
#include "MutMatrix.hpp"
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int, char const *[]) {
  std::ifstream infile("./test-cases/test-seq.txt");
  // std::ifstream infile("./test-cases/example-seq.txt");
  // std::ifstream infile("./test-cases/human-chimp-seq.txt");

  auto &ostrm = std::cout;

  std::string s1;
  std::string s2;
  std::getline(infile, s1);
  std::getline(infile, s2);

  alg::MutMatrix<int> scores(s1.size() + 1, s2.size() + 1);
  alg::MutMatrix<int> btrace(s1.size(), s2.size());
  auto matchPen = [](char a, char b) { return (a == b ? 2 : -2); };
  auto gapPen = []() { return -1; };

  alg::sequenceScoringNW<std::string, char, int>(s1, s2, scores, btrace,
                                                 matchPen, gapPen);

  if (scores.size() < 600) {
    for (auto const &mtx : {scores, btrace}) {
      for (size_t i = 0, sz = mtx.rowSize(); i < sz; ++i) {
        for (auto const &val : mtx.viewRow(i)) {
          ostrm << std::setw(3) << val << " ";
        }
        ostrm << std::endl;
      }
      ostrm << std::endl;
    }
  }

  auto seqVector =
      alg::findAlignmentsNW<std::string, char, int>(btrace, s1, s2);
  ostrm << seqVector.at(0).first << std::endl;
  ostrm << seqVector.at(0).second << std::endl;

  ostrm << "\nbest score: " << scores(s1.size(), s2.size());
  return 0;
}
