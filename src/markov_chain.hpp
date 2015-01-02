#ifndef SRC_MARKOV_CHAIN_HPP_
#define SRC_MARKOV_CHAIN_HPP_

#include <map>
#include <string>
#include <cctype>
#include <functional>
#include <vector>

namespace eagereye {

using std::map;
using std::string;
using std::vector;

typedef map<char, double> MarkovRow;
typedef std::pair<char, char> Bigram;
typedef vector<std::pair<char, char>> BigramList;

BigramList make_bigram_list(const string& word);

class MarkovChain : public map<char, MarkovRow> {
private:
  vector<char> acceptedChars_;

public:
  typedef std::function<bool(char)> CharAcceptor;
  double threshold_;

  MarkovChain(const string& file, CharAcceptor ca = [](const char& c){ return std::isprint(c);} );
  double probability(const string& word);
};

} /* namespace eagereye */

#endif
