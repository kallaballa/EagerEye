#include "population.hpp"
#include "genetic.hpp"
#include "error.hpp"
#include "markov_chain.hpp"
#include <boost/locale.hpp>
#include <fstream>
//#include <boost/regex.hpp>

#ifndef _NO_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#endif

#ifndef _NO_PROGRAM_OPTIONS
namespace po = boost::program_options;
#endif

namespace ee = eagereye;

using std::wstring;
using std::cerr;
using std::endl;
using std::vector;

/*boost::regex EMAIL_REGEX("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[A-Za-z]{2,4}$",boost::regex_constants::icase | boost::regex_constants::egrep);
boost::regex HTTP_URL_REGEX("http(s?):\\/\\/[^ \"\\(\\)\\<\\>]*",boost::regex_constants::icase | boost::regex_constants::egrep);
*/
bool filterMe(const wstring& word) {
  if(word.empty() || word.size() >= 64)
    return true;

/*
  if (boost::regex_match (word,EMAIL_REGEX))
    return true;


  if (boost::regex_match (word,HTTP_URL_REGEX))
    return true;
*/
  return false;
}

int main(int argc, char** argv) {
  //command line parsing
  string runPopFile;
  string trainPopFile;
  string loadMarkovFile;
  string genMarkovFile;
  string runMarkovFile;

  size_t gameIterations = 1000;

  #ifndef _NO_PROGRAM_OPTIONS
  po::options_description genericDesc("Options");
  genericDesc.add_options()
      ("iterations,i", po::value<size_t>(&gameIterations), "Run n iterations")
      ("run,r", po::value<string>(&runPopFile), "Load the population and run it on stdin")
      ("train,t", po::value<string>(&trainPopFile), "Train the population and save it. ")
      ("gen-markov,g", po::value<string>(&genMarkovFile),"Generate markov chain file")
      ("load-markov,m", po::value<string>(&loadMarkovFile),"Load the markov chain file")
      ("run-markov,n", po::value<string>(&runMarkovFile),"Load the markov chain file and validate words only using the markov chain")
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
  ee::ErrorHandler::init(ee::default_error_delegate);
  locale::global(locale("en_US.UTF-8"));
  std::locale loc = boost::locale::generator().generate("en_US.UTF-8");
  //std::wcin.imbue(loc);
  std::wcout.imbue(loc);

  CHECK(! (runPopFile.size() && trainPopFile.size()));
  CHECK(! (genMarkovFile.size() && loadMarkovFile.size()));

  srand(time(0));
  ee::PopulationLayout pl = ee::make_default_population_layout();
  ee::GeneticLayout gl = ee::make_default_genetic_layout();

  //make default genetic pools
  ee::GeneticPool pool = ee::GeneticPool(gl);
  ee::Population team;

  ee::MarkovChain mc;
  if(runMarkovFile.size()) {
    std::cerr << "run markov chain" << std::endl;
    ifstream is(runMarkovFile);
    ee::read_markov_chain(mc, is);
    wstring line;

    while(std::getline(std::wcin, line)) {
      std::wcerr << mc.probability(line) << '\t' << line << std::endl;
    }
  }

  if(!loadMarkovFile.size()) {
    mc.learn(std::wcin);
    if(genMarkovFile.size()) {
      ofstream os(genMarkovFile);
      ee::write_markov_chain(mc, os);
    }
  } else {
    ifstream is(loadMarkovFile);
    ee::read_markov_chain(mc, is);
  }


  //either load or create a team
  if (runPopFile.size()) {
    std::ifstream is(runPopFile);
    ee::read_population(0, team, is);
    wstring line;
    while (std::getline(std::wcin, line)) {
      if(filterMe(line))
        continue;

      team[0].brain_->reset();
      team[0].think(line, false, mc);

      std::wcout << round(team[0].brain_->outputs_[0]) << "\t" << line << std::endl;
    }
  } else if(trainPopFile.size()) {
    std::cerr << "Run training" << std::endl;
    team = ee::make_population(pl);
    wstring line;
    size_t cnt = 0;
    while (std::getline(std::wcin, line)) {
      bool isPass = false;
      if(line.at(0) == '1')
        isPass = true;
      else if(line.at(0) != '0' && line.at(0) != '2' && line.at(0) != '3')
        CHECK(false);

      line.erase (0,2);
      wstring candidate = line;
      if(filterMe(candidate))
        continue;

     // #pragma omp parallel for
      for(size_t i = 0; i < team.size(); ++i) {
        ee::Specimen& s = team[i];
        s.brain_->reset();
        s.think(candidate, isPass, mc);
      }

      ee::Population newTeam = pool.epoch(team);
      std::wcerr << round(team.stats_.averageFitness_) << "\t" << isPass << "\t" << mc.probability(line) << '\t' << candidate << std::endl;

      for(ee::Specimen& s : team) {
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
  }
}
