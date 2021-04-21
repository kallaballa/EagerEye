#ifndef TANK_HPP_
#define TANK_HPP_

#include <cstring>
#include <iostream>
#include "brain_fann.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

namespace phokis {

typedef BrainFann Brain;
using std::numeric_limits;

enum Action {
	BUY,
	SELL,
	PASS
};

class Specimen {
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
	Specimen(Brain* brain = NULL);
	~Specimen() {
	};

	void setBrain(Brain* b);

	Action think(const fann_type* data);
	Specimen makeChild() const;
	Specimen clone() const;
	bool operator<(const Specimen& other) const;
};

} /* namespace neurocid */

#endif /* TANK_HPP_ */
