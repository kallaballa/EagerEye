#ifndef CGENALG_H
#define CGENALG_H

#include "util.hpp"
#include "specimen.hpp"
#include "population.hpp"

using namespace std;

namespace phokis {

struct BattleFieldLayout;

struct GeneticLayout {
	double mutationRate;
	double crossoverRate;
	size_t crossoverIterations;
	double maxPertubation;
	size_t numElite_;
	size_t numEliteCopies_;
};

GeneticLayout make_default_genetic_layout();

//-----------------------------------------------------------------------
//
//	the genetic algorithm class
//-----------------------------------------------------------------------
class GeneticPool {
private:
	bool initialized_ = false;
	void mutate(Brain& brain);
	Specimen& pickSpecimen(Population& pop);
	Specimen* pickSpecimen(vector<Specimen*>& pop);
	std::pair<Specimen, Specimen> crossover(Specimen &mum, Specimen &dad, size_t iterations);
	void copyNBest(size_t n, const size_t numCopies, Population& in, Population& out);
	void calculateStatistics(Population& pop);
public:
	GeneticLayout layout_;
	GeneticPool(GeneticLayout params);
	GeneticPool();

	//this runs the GA for one generation.
	virtual Population epoch(Population& old_pop);
};
}

#endif

