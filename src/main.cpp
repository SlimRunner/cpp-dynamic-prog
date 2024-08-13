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

  size_t vertices = 0, edges = 0;
  vertices = s1.length() * s2.length();
  for (auto const &bt : btrace) {
    using namespace alg;
    for (auto const &dir : {Dir8::WEST, Dir8::NORTH, Dir8::NORTH_WEST}) {
      if (bt & static_cast<int>(dir)) {
        ++edges;
      }
    }
  }

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

  constexpr const auto branchLimit = 10;
  auto seqVector = alg::findAlignmentsNW<std::string, char, int>(btrace, s1, s2,
                                                                 branchLimit);
  ostrm << "best score: " << scores(s1.size(), s2.size()) << "\n";
  ostrm << "back trace has " << vertices << " nodes and " << edges << " edges\n";
  ostrm << "paths     : " << seqVector.size();
  ostrm << " (" << branchLimit << " limit)";
  ostrm << "\n" << std::endl;
  for (auto const &seq : seqVector) {
    ostrm << seq.first << std::endl;
    ostrm << seq.second << "\n" << std::endl;
  }
  return 0;
}
