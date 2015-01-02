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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace eagereye {

using namespace cv;

Specimen::Specimen(Brain* brain) :
    brain_(brain),
    fitness_(0) {
}

void Specimen::setBrain(Brain* b) {
	CHECK(brain_ == NULL);
	brain_ = b;
}

struct WordStats {
  size_t alpha = 0;
  size_t digit = 0;
  size_t upper = 0;
  size_t blank = 0;
  size_t punct = 0;
  size_t xdigit = 0;
};

//FIXME
WordStats make_word_stats(const std::string& word) {
  using namespace std;

  WordStats stats;

  for(const char& c : word)
    stats.alpha += (isalpha(c) ? 1 : 0);

  for(const char& c : word)
    stats.digit += (isdigit(c) ? 1 : 0);

  for(const char& c : word)
    stats.upper += (isupper(c) ? 1 : 0);

  for(const char& c : word)
    stats.blank += (isblank(c) ? 1 : 0);

  for(const char& c : word)
    stats.punct += (ispunct(c) ? 1 : 0);

  for(const char& c : word)
    stats.xdigit += (isxdigit(c) ? 1 : 0);

  return stats;
}

Mat make_mat(vector<double> v) {
  Mat mat = Mat(Size(v.size(), 1), CV_64F);
  size_t cnt = 0;
  MatIterator_<double> it, end;

  for( it = mat.begin<double>(), end = mat.end<double>(); it != end; ++it) {
    *it = v[cnt++];
  }

  return mat;
}

vector <double> make_vector(Mat m) {
  MatIterator_<double> it, end;
  vector<double> result;
  for( it = m.begin<double>(), end = m.end<double>(); it != end; ++it) {
    result.push_back(*it);
  }
  return result;
}


vector<double> make_gradient(const string& word, size_t len) {
  vector<double> vecWord;
  for(const unsigned char& c : word) {
    vecWord.push_back((double) c/127.0);
  }

  Mat matIn = make_mat(vecWord);
  Mat matOut;
  resize(matIn, matOut, Size(len, 1));
  auto result = make_vector(matOut);
/*
  std::cerr << "vecWord:";
  for (const double& d : vecWord) {
    std::cerr << d << '\t';
  }
  std::cerr << std::endl;

  std::cerr << "result:";
  for (const double& d : result) {
    std::cerr << d << '\t';
  }
  std::cerr << std::endl;*/

  return result;
}

void Specimen::think(const std::string& candidate, bool isPass, MarkovChain& mc) {
  CHECK(candidate.size() < 64);
  CHECK(brain_->layout_.numOutputs == 1);
  CHECK(!candidate.empty());
  size_t cnt = 0;


  WordStats stats = make_word_stats(candidate);
  double len = candidate.size();
  std::vector<double> inputs;
  inputs.push_back(mc.probability(candidate));

  inputs.push_back(1.0 - (((double)stats.alpha / len) * 2.0));
  inputs.push_back(1.0 - (((double)stats.digit / len) * 2.0));
  inputs.push_back(1.0 - (((double)stats.upper / len) * 2.0));
  inputs.push_back(1.0 - (((double)stats.blank / len) * 2.0));
  inputs.push_back(1.0 - (((double)stats.punct / len) * 2.0));
  inputs.push_back(1.0 - (((double)stats.xdigit / len) * 2.0));
  inputs.push_back(1.0 - (len / 64.0));

  for(size_t i = 0; i < inputs.size(); ++i) {
/*    if(!(inputs[i] <= 1.0 && inputs[i] >= -1.0)) {
      std::cerr << inputs[i] << std::endl;
      CHECK(false);
    }*/
    brain_->inputs_[cnt++] = inputs[i];
  }

  /*auto grad = make_gradient(candidate, 64);

  for(const double& d : grad) {
    brain_->inputs_[cnt++] = d;
  }*/

  brain_->run();

	double expected = isPass ? 1.0 : -1.0;
	fitness_ = 2.0 - std::fabs(expected - brain_->outputs_[0]);
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
