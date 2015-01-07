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
using std::wstring;
using std::vector;

typedef map<wchar_t, double> MarkovRow;
typedef std::pair<wchar_t, wchar_t> Bigram;
typedef vector<std::pair<wchar_t, wchar_t>> BigramList;

BigramList make_bigram_list(const wstring& word);

class MarkovChain : public map<wchar_t, MarkovRow> {
private:
#ifndef _NO_SERIALIZE
    friend class boost::serialization::access;
#endif
  vector<wchar_t> acceptedChars_;
  std::mutex lookupMutex_;
  double maxProbability_;
public:
  typedef std::function<bool(wchar_t)> CharAcceptor;

  MarkovChain() {};
  void learn(const std::string& file);
  void learn(std::wistream& is);
  double probability(const wstring& word);

#ifndef _NO_SERIALIZE
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & acceptedChars_;
    ar & maxProbability_;
    ar & boost::serialization::base_object<map<wchar_t,MarkovRow>>(*this);
  }
#endif
};

void read_markov_chain(MarkovChain& mc, std::istream& is);
void write_markov_chain(MarkovChain& mc, std::ostream& os);
} /* namespace eagereye */

#endif
