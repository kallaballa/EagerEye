#include "genetic.hpp"
#include "fann.h"
#include "util.hpp"
#include "error.hpp"
#include <algorithm>
#include <iostream>
#include <set>

namespace phokis {

GeneticLayout make_default_genetic_layout() {
  return {
    0.05, // mutationRate
    0.7,// crossoverRate
    4,// crossoverIterations
    0.3,// maxPertubation
    4,// numElite
    1,// numEliteCopies
  };
}

using std::set;

GeneticPool::GeneticPool(GeneticLayout params) :
    layout_(params) {
  initialized_ = true;
}

GeneticPool::GeneticPool() :
    layout_() {
  initialized_ = false;
}

/*
 * mutates a chromosome by perturbing its weights by an amount not greater than params_.maxPerturbation_
 */
void GeneticPool::mutate(Brain& brain) {
  //traverse the chromosome and mutate each weight dependent
  //on the mutation rate

    fann_type* weights = brain.weights();
    for (size_t i = 0; i < brain.size(); ++i) {
      //do we perturb this weight?
      if (fRand(0, 1) < layout_.mutationRate) {
        //add or subtract a small value to the weight
        weights[i] += ((fRand(0, 1) - fRand(0, 1)) * layout_.maxPertubation);
      }
    }
}

/*
 * Returns a ship base on roulette wheel sampling
 */
Specimen& GeneticPool::pickSpecimen(Population& pop) {
  double minFitness = 99999999999;
  for (size_t i = 0; i < pop.size(); ++i) {
    minFitness = std::min(minFitness, pop[i].fitness_);
  }

  //generate a random number between 0 & total fitness count
  double slice = (double) fRand(0, pop.stats_.totalFitness_ - (minFitness * pop.size()));

  double fitnessSoFar = 0;
  for (size_t i = 0; i < pop.size(); ++i) {
    fitnessSoFar += (pop[i].fitness_ - minFitness);

    //if the fitness so far > random number return the tank at this point
    if (fitnessSoFar >= slice) {
      return pop[i];
    }
  }

  CHECK(false);
  return pop[0]; 	//surpress warning
}

/*
 * Returns a ship based on roulette wheel sampling
 */
Specimen* GeneticPool::pickSpecimen(vector<Specimen*>& pop) {
  double totalFitness = 0;
  double minFitness = 99999999999;

  for (Specimen* s : pop) {
    totalFitness += s->fitness_;
    minFitness = std::min(minFitness, s->fitness_);
  }

  //generate a random number between 0 & total fitness count
  double slice = (double) (fRand(0, totalFitness - (minFitness * pop.size())));

  //this will be set to the chosen tank go through the tanks adding up the fitness so far
  double fitnessSoFar = 0;

  for (size_t i = 0; i < pop.size(); ++i) {
    fitnessSoFar += (pop[i]->fitness_ - minFitness);

    //if the fitness so far > random number return the tank at this point
    if (fitnessSoFar >= slice) {
      return pop[i];
    }
  }

  CHECK(false);
  return pop[0]; 	//surpress warning
}

/*
 * With a chance defined by params_.crossoverRate_ perform a crossover of brain_.weights()
 */
std::pair<Specimen, Specimen> GeneticPool::crossover(Specimen &mum, Specimen &dad, size_t iterations) {
  Specimen baby1 = mum.makeChild();
  Specimen baby2 = mum.makeChild();

    fann_type* wMum = mum.brain_->weights();
    fann_type* wDad = dad.brain_->weights();
    fann_type* wBaby1 = baby1.brain_->weights();
    fann_type* wBaby2 = baby2.brain_->weights();

    if ((fRand(0, 1) > layout_.crossoverRate)) {
      for (size_t i = 0; i < mum.brain_->size(); ++i) {
        wBaby1[i] = wMum[i];
        wBaby2[i] = wDad[i];
      }
      return {baby1, baby2};
    }

    size_t last_cp = 0;
    bool cross = false;

    //generate up to "iterations" crossover points and perform them in ascending order
    set<size_t> crossPoints;

    for (size_t i = 0; i < iterations; ++i) {
      crossPoints.insert(iRand(0, mum.brain_->size() - 1));
    }

    for (const size_t& cp : crossPoints) {
      //create the offspring
      for (size_t j = last_cp; j < cp; ++j) {
        if (cross) {
          wBaby1[j] = wMum[j];
          wBaby2[j] = wDad[j];
        } else {
          wBaby1[j] = wDad[j];
          wBaby2[j] = wMum[j];
        }
      }

      last_cp = cp;
      cross = !cross;
    }

    for (size_t i = last_cp; i < mum.brain_->size(); ++i) {
      if (cross) {
        wBaby1[i] = wMum[i];
        wBaby2[i] = wDad[i];
      } else {
        wBaby1[i] = wDad[i];
        wBaby2[i] = wMum[i];
      }
    }

  return {baby1, baby2};
}

/*
 * copy numCopies copies of the n best specimen into the out population
 */
void GeneticPool::copyNBest(size_t n, const size_t numCopies, Population& in, Population& out) {
  //add the required amount of copies of the n most fittest to the supplied population
  while (n--) {
    for (size_t i = 0; i < numCopies; ++i) {
      Specimen& t = in[(in.size() - 1) - n];
      Specimen clone = t.clone();
      out.push_back(clone);
    }
  }
}

/*
 * generates the statistics for the given population
 */
void GeneticPool::calculateStatistics(Population& pop) {

  size_t size = pop.size();
  double highestSoFar = 0;
  double lowestSoFar = 9999999;

  pop.stats_.bestFitness_ = std::numeric_limits<double>().max();

  for (size_t i = 0; i < size; ++i) {

    //update fittest if necessary
    if (highestSoFar == 0 || pop[i].fitness_ > highestSoFar) {
      highestSoFar = pop[i].fitness_;
      pop.stats_.fittestGenome_ = i;
      pop.stats_.bestFitness_ = highestSoFar;
    }

    //update worst if necessary
    if (pop[i].fitness_ < lowestSoFar) {
      lowestSoFar = pop[i].fitness_;
      pop.stats_.worstFitness_ = lowestSoFar;
    }

    pop.stats_.totalFitness_ += pop[i].fitness_;
  }

  CHECK(pop.stats_.bestFitness_ != std::numeric_limits<double>().max());

  pop.stats_.averageFitness_ = pop.stats_.totalFitness_ / size;
}

/*
 * Use the genetic algorithm to construct a new population from the old
 */
Population GeneticPool::epoch(Population& old_pop) {
  if (!initialized_ || old_pop.size() == 1) {
    Population new_pop = old_pop;
    new_pop.clear();
    old_pop.stats_.reset();

    //sort the population (for scaling and elitism)
    sort(old_pop.begin(), old_pop.end());

    //calculate best, worst, average and total fitness
    calculateStatistics(old_pop);

    for (Specimen& t : old_pop) {
      new_pop.push_back(t.makeChild());
    }
    new_pop.stats_ = old_pop.stats_;
    return new_pop;
  }

  Population new_pop = old_pop;
  new_pop.clear();
  old_pop.stats_.reset();

  //sort the population (for scaling and elitism)
  sort(old_pop.begin(), old_pop.end());

  //calculate best, worst, average and total fitness
  calculateStatistics(old_pop);

  /*
   * Make sure we add an EVEN number or the roulette wheel sampling will crash
   */
  if (layout_.numElite_ < old_pop.size()) {
    if (!(layout_.numEliteCopies_ * (layout_.numElite_ % 2))) {
      copyNBest(layout_.numElite_, layout_.numEliteCopies_, old_pop, new_pop);
    }
  }

  //now we enter the GA loop
  //repeat until a new population is generated
  while (new_pop.size() < old_pop.size()) {
    //grab two chromosones
    Specimen& mum = pickSpecimen(old_pop);
    Specimen* dad = &pickSpecimen(old_pop);

    //create some offspring via crossover
    std::pair<Specimen, Specimen> babies = crossover(mum, *dad, layout_.crossoverIterations);

    //now we mutate
    mutate(*babies.first.brain_);
    mutate(*babies.second.brain_);

    //now copy into vecNewPop population
    new_pop.push_back(babies.first);
    if (new_pop.size() < old_pop.size())
      new_pop.push_back(babies.second);
  }
  CHECK(new_pop.size() == old_pop.size());
  old_pop.stats_.generationCnt_++;
  new_pop.stats_ = old_pop.stats_;
  return new_pop;
}
}
