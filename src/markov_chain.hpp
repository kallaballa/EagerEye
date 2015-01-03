#ifndef SRC_MARKOV_CHAIN_HPP_
#define SRC_MARKOV_CHAIN_HPP_

#include <map>
#include <string>
#include <cctype>
#include <functional>
#include <vector>
#include <mutex>
#include <iostream>
#ifndef _NO_SERIALIZE
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace eagereye {

using std::map;
using std::string;
using std::vector;

typedef map<uint8_t, double> MarkovRow;
typedef std::pair<uint8_t, uint8_t> Bigram;
typedef vector<std::pair<uint8_t, uint8_t>> BigramList;

BigramList make_bigram_list(const string& word);

class MarkovChain : public map<uint8_t, MarkovRow> {
private:
#ifndef _NO_SERIALIZE
    friend class boost::serialization::access;
#endif
  vector<uint8_t> acceptedChars_;
  std::mutex lookupMutex_;
  double maxProbability_;
public:
  typedef std::function<bool(uint8_t)> CharAcceptor;

  MarkovChain() {};
  void learn(const string& file, CharAcceptor ca = [](const uint8_t& c){ return std::isprint(c);} );
  double probability(const string& word);

#ifndef _NO_SERIALIZE
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & acceptedChars_;
    ar & maxProbability_;
    ar & boost::serialization::base_object<map<uint8_t,MarkovRow>>(*this);
  }
#endif
};

void read_markov_chain(MarkovChain& mc, std::istream& is);
void write_markov_chain(MarkovChain& mc, std::ostream& os);
} /* namespace eagereye */

#endif
