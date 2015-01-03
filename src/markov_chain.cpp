#include "markov_chain.hpp"
#include "error.hpp"
#include <fstream>
#include <numeric>
#include <algorithm>
#include <iostream>

namespace eagereye {

BigramList make_bigram_list(const string& word) {
  if(word.size() < 2) {
    return BigramList();
  }

  BigramList bl(word.size() - 1);

  for (size_t i = 1; i < word.size(); ++i) {
    bl[i - 1] = { word.at(i - 1), word.at(i) };
  }

  return bl;
}

void MarkovChain::learn(const string& file, CharAcceptor ca) {
  std::cerr << "training markov chain" << std::endl;

  if (acceptedChars_.empty()) {
    for (size_t i = 0; i < 256; ++i) {
      if (ca(i)) {
        acceptedChars_.push_back((uint8_t)i);
      }
    }
  }

  for (uint8_t& i : acceptedChars_) {
    for (uint8_t& j : acceptedChars_) {
      (*this)[i][j] = 10.0;
    }
  }

  std::ifstream is(file);
  string line;

  size_t cnt = 0;
  while (std::getline(is, line)) {
    for (Bigram& b : make_bigram_list(line)) {
      ++((*this)[b.first][b.second]);
    }
    std::cerr << "\r" << ++cnt;
  }

  //normalize
  double maxRelativeProbability = 0;
  for (uint8_t& i : acceptedChars_) {
    MarkovRow& row = (*this)[i];

    double sum = 0;
    for (uint8_t& j : acceptedChars_) {
      sum += row[j];
      CHECK(row[j] > 0);
    }

    for (uint8_t& j : acceptedChars_) {
        row[j] = row[j]/sum;
        maxRelativeProbability = std::max(maxRelativeProbability, row[j]);
    }
  }

  for (uint8_t& i : acceptedChars_) {
    MarkovRow& row = (*this)[i];

    for (uint8_t& j : acceptedChars_) {
      row[j] /= maxRelativeProbability;
      CHECK(row[j] >= 0.0 && row[j] <= 1.0);
    }
  }
}

double MarkovChain::probability(const string& word) {
  std::unique_lock<std::mutex> lck(lookupMutex_);
  double totalProb = 0.0;
  size_t cnt = 0;

  for (const uint8_t& i : word) {
    if(std::find(acceptedChars_.begin(), acceptedChars_.end(), i) == acceptedChars_.end()) {
      return 0;
    }
  }

  for (Bigram& b : make_bigram_list(word)) {
    totalProb += (*this)[b.first][b.second];
    ++cnt;
  }

  if(cnt == 0)
    return 0;

  return totalProb / (double)cnt;
}

void read_markov_chain(MarkovChain& mc, std::istream& is) {
#ifndef _NO_SERIALIZE
  boost::archive::binary_iarchive ia(is);
  ia >> mc;
#else
  CHECK(false);
#endif
}

void write_markov_chain(MarkovChain& mc, std::ostream& os) {
#ifndef _NO_SERIALIZE
  boost::archive::binary_oarchive oa(os);
  oa << mc;
#else
  CHECK(false);
#endif
}
} /* namespace eagereye */
