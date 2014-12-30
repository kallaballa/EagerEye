#ifndef TANK_HPP_
#define TANK_HPP_

#include <cstring>
#include <iostream>
#include "brain_fann.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace eagereye {

typedef BrainFann Brain;
using std::numeric_limits;

class Ship {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif

public:
	Brain* brain_;
	double fitness_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & brain_;
	}
#endif
	Ship(Brain* brain = NULL);
	~Ship() {
	};

	void setBrain(Brain* b);

	void think(const std::string& candidate, bool isPass);
	Ship makeChild() const;
	Ship clone() const;
	bool operator<(const Ship& other) const;
};

} /* namespace neurocid */

#endif /* TANK_HPP_ */
