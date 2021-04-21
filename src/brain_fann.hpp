#ifndef BRAIN_FANN_HPP_
#define BRAIN_FANN_HPP_

#include <fann.h>
#include <memory>
#include <vector>
#include <deque>
#include "brain.hpp"

#ifndef _NO_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/deque.hpp>
#endif

namespace phokis {

class Specimen;
class Population;

using std::deque;

class BrainFann : public BasicBrain<fann_type> {
	fann *nn_;
public:
	typedef fann_type value_type;

	BrainFann() : BasicBrain<fann_type>(), nn_(NULL) {
	}

	BrainFann(const BrainFann& other);
	virtual ~BrainFann();

	virtual void makeNN();
	virtual void applyInput(const size_t& i, const fann_type& value);
	virtual void destroy();
	virtual void randomize();
	virtual void reset();
	virtual size_t size() const override;
	virtual fann_type* weights() override;

	virtual bool operator==(BrainFann& other);
	virtual bool operator!=(BrainFann& other);
	void run();
};

} /* namespace neurocid */

#endif /* BRAIN_FANN */
