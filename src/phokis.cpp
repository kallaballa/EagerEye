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

namespace phk = phokis;

using std::cerr;
using std::endl;
using std::vector;

int main(int argc, char** argv) {
  //command line parsing
  string runPopFile;
  string trainPopFile;

  size_t gameIterations = 1000;

  #ifndef _NO_PROGRAM_OPTIONS
  po::options_description genericDesc("Options");
  genericDesc.add_options()
      ("iterations,i", po::value<size_t>(&gameIterations), "Run n iterations")
      ("run,r", po::value<string>(&runPopFile), "Load the population and run it on stdin")
      ("train,t", po::value<string>(&trainPopFile), "Train the population and save it. ")
      ("help,h", "Produce help message");


  po::options_description cmdline_options;
  cmdline_options.add(genericDesc);

  po::options_description visible;
  visible.add(genericDesc);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << "Usage: eagereye [options] <population file>\n";
    std::cerr << visible;
    return 0;
  }

#endif
  phk::ErrorHandler::init(phk::default_error_delegate);

  CHECK(! (runPopFile.size() && trainPopFile.size()));

  srand(time(0));
  phk::PopulationLayout pl = phk::make_default_population_layout();
  phk::GeneticLayout gl = phk::make_default_genetic_layout();

  //make default genetic pools
  phk::GeneticPool pool = phk::GeneticPool(gl);
  phk::Population team;

  //either load or create a team
  if (runPopFile.size()) {

  } else if(trainPopFile.size()) {
    std::cerr << "Run training" << std::endl;
    team = phk::make_population(pl);
/*
     // #pragma omp parallel for
      for(size_t i = 0; i < team.size(); ++i) {
        phk::Specimen& s = team[i];
        s.brain_->reset();
        s.think(nullptr);
      }

      phk::Population newTeam = pool.epoch(team);
      std::wcerr << round(team.stats_.averageFitness_) << std::endl;

      for(phk::Specimen& s : team) {
        s.brain_->destroy();
      }

      newTeam.stats_.print(std::cout);
      std::cout << std::endl;
      team = newTeam;

      if(cnt % 1000 == 0) {
        std::ofstream os(trainPopFile);
        write_population(team,os);
      }
      ++cnt;
    }
    std::ofstream os(runPopFile);
    write_population(team,os);
 */
  }
}
