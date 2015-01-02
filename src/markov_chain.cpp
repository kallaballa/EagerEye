#include "markov_chain.hpp"
#include "error.hpp"
#include <fstream>
#include <numeric>
#include <algorithm>
#include <iostream>

namespace eagereye {

BigramList make_bigram_list(const string& word) {
  BigramList bl;
  bool first = true;
  char last;
  string normalized = word;

  for(char& c : normalized) {
    c = std::tolower(c);
  }

  for (const char& c : normalized) {
    if (first) {
      first = false;
    } else {
      bl.push_back( { last, c });
    }
    last = c;
  }

  return bl;
}

MarkovChain::MarkovChain(const string& file, CharAcceptor ca) {
  std::cerr << "training markov chain" << std::endl;

  if (acceptedChars_.empty()) {
    for (size_t i = 0; i < 256; ++i) {
      if (ca(i)) {
        acceptedChars_.push_back((char)i);
      }
    }
  }

  for (char& i : acceptedChars_) {
    (*this).insert({i, MarkovRow()});
    for (char& j : acceptedChars_) {
      (*this)[i].insert({j, 10.0});
    }
  }

  std::ifstream is(file);
  string line;

  while (std::getline(is, line)) {
    for (Bigram& b : make_bigram_list(line)) {
      ++((*this)[b.first][b.second]);
    }
  }

  for (char& i : acceptedChars_) {
    MarkovRow& row = (*this)[i];

    double sum = 0;
    for (auto& p2 : row) {
      sum += p2.second;
    }

    CHECK(sum > 0);

    for (char& j : acceptedChars_) {
      double& count = row[j];
      CHECK(count > 0);
      count = log((double)count / (double)sum);
    }
  }

}

double MarkovChain::probability(const string& word) {
  double log_prob = 0.0;
  size_t transition_cnt = 0;

  for (Bigram& b : make_bigram_list(word)) {
    log_prob += (*this)[b.first][b.second];
    ++transition_cnt;
  }

  return exp(log_prob / (double)(transition_cnt == 0.0 ? 1.0 : transition_cnt));
}
} /* namespace eagereye */
