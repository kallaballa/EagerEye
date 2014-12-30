#ifndef POPULATION_HPP_
#define POPULATION_HPP_

#include <vector>
#include <iostream>
#include "ship.hpp"
#include "error.hpp"

#ifndef _NO_SERIALIZE
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace eagereye {
using std::vector;
using std::istream;
using std::ostream;

struct PopulationLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	size_t size_;
	BrainLayout bl_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    ar & size_;
	  ar & bl_;
	}
#endif
};

PopulationLayout make_default_population_layout();

class Population: public vector<Ship> {
#ifndef _NO_SERIALIZE
	  friend class boost::serialization::access;
#endif

public:
	struct Statistics {
		Statistics() : generationCnt_(0) {
			reset();
		}

		//total fitness of population
		double totalFitness_;
		//best fitness this population
		double bestFitness_;
		//average fitness
		double averageFitness_;
		//worst
		double worstFitness_;

		size_t generationCnt_;

		size_t fittestGenome_;

		void reset() {
			totalFitness_ = 0;
			bestFitness_ = 0;
			worstFitness_ = std::numeric_limits<double>().max();
			averageFitness_ = 0;
			totalFitness_ = 0;
		}

		void print(std::ostream& os) {
			os << generationCnt_ << ":"
					<< bestFitness_ << ":"
					<< averageFitness_;
		}
	};

	PopulationLayout layout_;
	Statistics stats_;

	Population& operator=(const Population& other) {
		layout_ = other.layout_;
		stats_ = other.stats_;
		vector<Ship>::operator=(other);
		return *this;
	}

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & layout_;
	  ar & *((vector<Ship>*)this);
	}
#endif
};

/* FIXME this is a very simple and stupid scaling mechanism.
 * scenarios should avoid scaling down a lot and scenarios
 * that scale up heavily need quite some time to adapt.
 */
inline void scale_population(Population& team, size_t size) {
	if (team.size() > size) {
		for (size_t i = size; i < team.size(); ++i) {
			team[i].brain_->destroy();
		}
		team.resize(size);
	} else if (team.size() < size) {
		while (team.size() < size) {
			for (Ship& s : team) {
				team.push_back(s.clone());
				if (team.size() == size)
					break;
			}
		}
	}
}

inline void read_population(size_t teamID, Population& team, istream& is) {
#ifndef _NO_SERIALIZE
  boost::archive::binary_iarchive ia(is);
  ia >> team;

  scale_population(team, team.layout_.size_);
#else
  CHECK(false);
#endif
}

inline void write_population(Population& team, ostream& os) {
#ifndef _NO_SERIALIZE
  boost::archive::binary_oarchive oa(os);
  oa << team;
#else
  CHECK(false);
#endif
}
}

#endif /* POPULATION_HPP_ */
