#include "brain_fann.hpp"
#include "error.hpp"
#include <fann.h>
#include <iostream>
#include <limits>
#include <cmath>

namespace eagereye {

BrainFann::BrainFann(const BrainFann& other): BasicBrain<fann_type>(other), nn_(other.nn_) {
}

BrainFann::~BrainFann() {
}

void BrainFann::applyInput(const size_t& i, const fann_type& value) {
	CHECK(i < layout_.numInputs_);
	inputs_[i] = value;
	CHECK(!std::isnan(inputs_[i]) && !std::isinf(inputs_[i]) && inputs_[i] >= -1 && inputs_[i] <= 1);
}

void BrainFann::makeNN() {
	CHECK(layout_.numLayers_ >= 3);
	CHECK(layout_.numLayers_ < 20);

	unsigned int* layerArray = new unsigned int[layout_.numLayers_];

  // create sub brains
  layerArray[0] = layout_.numInputs_;
	for(size_t i = 1; i < (layout_.numLayers_ - 1); i++) {
		layerArray[i] = layout_.numNeuronsPerHidden_;
	}
  layerArray[layout_.numLayers_ - 1] = layout_.numOutputs;

  nn_ = fann_create_standard_array(layout_.numLayers_, layerArray);
	fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);

	inputs_ = new fann_type[layout_.numInputs_];
  outputs_ = new fann_type[layout_.numOutputs];

	delete[] layerArray;

	reset();

}

void BrainFann::destroy() {
	CHECK(!destroyed_);
	CHECK(nn_ != NULL);
	fann_destroy(nn_);
	nn_ = NULL;

	delete[] inputs_;
	inputs_ = NULL;

  delete[] outputs_;
  outputs_ = NULL;

  destroyed_ = true;
}

void BrainFann::reset() {
	CHECK(inputs_ != NULL);
  CHECK(outputs_ != NULL);

	std::fill_n(inputs_, layout_.numInputs_, 0);
}

void BrainFann::randomize() {
  fann_randomize_weights(nn_, -1, 1);
}

size_t BrainFann::size() const {
	return nn_->total_connections;
}

fann_type* BrainFann::weights() {
	return nn_->weights;
}

bool BrainFann::operator==(BrainFann& other) {
  CHECK(other.size() == this->size());
  for(size_t j = 0; j < this->size(); ++j){
    if(this->weights()[j] != other.weights()[j])
      return false;
  }
	return true;
}

bool BrainFann::operator!=(BrainFann& other) {
	return !operator==(other);
}

void BrainFann::run() {
	fann_type* outputs = fann_run(nn_, inputs_);
	for(size_t i = 0; i < layout_.numOutputs; ++i) {
	  outputs_[i] = outputs[i];
	  CHECK(!std::isnan(outputs_[i]) && !std::isnan(outputs_[i]));
	}
}
} /* namespace neurocid */
