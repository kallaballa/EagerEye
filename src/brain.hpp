#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <fann.h>
#include <memory>
#include <vector>

#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace eagereye {

struct BrainLayout  {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	size_t numInputs_;
	size_t numOutputs;
	size_t numLayers_;
	size_t numNeuronsPerHidden_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & numInputs_;
		ar & numOutputs;
		ar & numLayers_;
		ar & numNeuronsPerHidden_;
	}
#endif
};

class Ship;
class Population;

template<typename Tweight> class BasicBrain {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
public:
	bool destroyed_ = false;
	bool initialized_ = false;
	BrainLayout  layout_;
	Tweight* inputs_ = NULL;
  Tweight* outputs_ = NULL;

  BasicBrain() {
	}

	void initialize(BrainLayout layout, Tweight* weight = NULL) {
	  outputs_ = new Tweight[layout.numOutputs];
	  layout_ = layout;
		inputs_ = NULL;
		makeNN();
	    if(weight != NULL) {
				for(size_t i = 0; i < size(); ++i) {
					weights()[i] = weight[i];
				}
	    }
	    initialized_ = true;
	}

	bool isDestroyed() {
		return destroyed_;
	}

	BasicBrain(const BasicBrain& other) : inputs_(other.inputs_) {
	};

	virtual ~BasicBrain() {
	};

	virtual void makeNN() = 0;
	virtual void applyInput(const size_t& i, const fann_type& value) = 0;
	virtual void destroy() = 0;
	virtual void randomize() = 0;
	virtual void reset() = 0;
	virtual size_t size() const = 0;
	virtual Tweight* weights() = 0;
#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & layout_;
	  if(!initialized_)
		  makeNN();

	  ar & boost::serialization::make_array(weights(), size());
	}
#endif
};

} /* namespace neurocid */

#endif /* BRAIN_HPP_ */
