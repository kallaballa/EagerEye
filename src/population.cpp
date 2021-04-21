
#include "population.hpp"

namespace phokis {

PopulationLayout make_default_population_layout() {
	return {
		20, // size_
		//BrainLayout
		{
		  24,// inputs
			2,  // outputs
			5,  // layers
			8, // neurons per hidden layer
		},
	};
}

}
