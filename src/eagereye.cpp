#include "population.hpp"
#include "genetic.hpp"
#include "error.hpp"

#include <fstream>

#ifndef _NO_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#endif

#ifndef _NO_PROGRAM_OPTIONS
namespace po = boost::program_options;
#endif

namespace ee = eagereye;

using std::string;
using std::cerr;
using std::endl;
using std::vector;

int main(int argc, char** argv) {
  //command line parsing
  string popFile = "default";
  size_t gameIterations = 1000;
  bool runPop = false;
  bool trainPop = false;

#ifndef _NO_PROGRAM_OPTIONS
  po::options_description genericDesc("Options");
  genericDesc.add_options()
      ("iterations,i", po::value<size_t>(&gameIterations), "Run n iterations")
      ("run,r", "Load the population and run it on stdin")
      ("train,t", "Train the population and save it. ")
      ("help,h", "Produce help message");

  po::options_description hidden("Hidden options");
  hidden.add_options()("popFile", po::value<string>(&popFile), "popFile");

  po::options_description cmdline_options;
  cmdline_options.add(genericDesc).add(hidden);

  po::positional_options_description p;
  p.add("popFile", -1);

  po::options_description visible;
  visible.add(genericDesc);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << "Usage: eagereye [options] <population file>\n";
    std::cerr << visible;
    return 0;
  }

  runPop = vm.count("run");
  trainPop = vm.count("train");
#endif
  ee::ErrorHandler::init(ee::default_error_delegate);

  CHECK(! (runPop && trainPop));
  CHECK(runPop || trainPop);


  srand(time(0));
  ee::PopulationLayout pl = ee::make_default_population_layout();
  ee::GeneticLayout gl = ee::make_default_genetic_layout();

  //make default genetic pools
  ee::GeneticPool pool = ee::GeneticPool(gl);
  ee::Population team;

  //either load or create a team
  if (runPop) {
    std::ifstream is(popFile);
    ee::read_population(0, team, is);
    string line;
    while (std::getline(std::cin, line)) {
      team[0].brain_->reset();
      team[0].think(line, false);
      std::cout << team[0].brain_->outputs_[0] << std::endl;
    }
  } else if(trainPop) {
    std::cerr << "Run training" << std::endl;
    team = ee::make_population(pl);
    string line;
    size_t cnt = 0;
    while (std::getline(std::cin, line)) {
      bool isPass;
      if(line.at(0) == '0')
        isPass = false;
      else if(line.at(0) == '1')
        isPass = true;
      else
        CHECK(false);

      line.erase (0,2);
      string candidate = line;
      if(candidate.empty())
        continue;

      for(ee::Specimen& s : team) {
        s.brain_->reset();
        s.think(candidate, isPass);
      }

      std::cerr << round(team.stats_.averageFitness_) << "\t" << isPass << "\t" << candidate << std::endl;

      ee::Population newTeam = pool.epoch(team);

      for(ee::Specimen& s : team) {
        s.brain_->destroy();
      }

      newTeam.stats_.print(std::cout);
      std::cout << std::endl;
      team = newTeam;

      if(cnt % 1000 == 0) {
        std::ofstream os(popFile);
        write_population(team,os);
      }
      ++cnt;
    }
    std::ofstream os(popFile);
    write_population(team,os);
  }
}
