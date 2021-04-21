#include "specimen.hpp"
#include "population.hpp"
#include "error.hpp"
#include <cstdlib>
#include <iostream>
#include "util.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <cctype>

namespace phokis {

Specimen::Specimen(Brain* brain) :
    brain_(brain),
    fitness_(0) {
}

void Specimen::setBrain(Brain* b) {
	CHECK(brain_ == NULL);
	brain_ = b;
}


Action Specimen::think(const fann_type* data) {
	CHECK(brain_->layout_.numOutputs == 2);

	for (size_t i = 0; i < brain_->layout_.numInputs_; ++i) {
		const fann_type& v = data[i];
		if (!(v <= 1.0 && v >= -1.0)) {
			std::cerr << v << std::endl;
			CHECK(false);
		}
		brain_->inputs_[i] = v;
	}

	brain_->run();

	if (brain_->outputs_[0] && brain_->outputs_[1]) {
		return PASS;
	} else if (brain_->outputs_[0]) {
		return BUY;
	} else if (brain_->outputs_[1]) {
		return SELL;
	} else {
		return PASS;
	}

}

Specimen Specimen::makeChild() const {
	CHECK(brain_ != NULL);
	Specimen child;
	Brain* fresh  = new Brain();
	fresh->initialize(brain_->layout_);
	child.setBrain(fresh);
	return child;
}

Specimen Specimen::clone() const {
	CHECK(brain_ != NULL);
	Specimen child;
	Brain* fresh  = new Brain();
	fresh->initialize(brain_->layout_);
	child.setBrain(fresh);

	//copy brain
  for(size_t i = 0; i < brain_->size(); ++i) {
    child.brain_->weights()[i] = brain_->weights()[i];
  }
	return child;
}

bool Specimen::operator<(const Specimen& other) const {
	return (this->fitness_ < other.fitness_);
}

} /* namespace neurocid */
