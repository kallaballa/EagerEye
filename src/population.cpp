
#include "population.hpp"

namespace eagereye {

PopulationLayout make_default_population_layout() {
	return {
		20, // size_
		//BrainLayout
		{
		  8,// inputs
			1,  // outputs
			5,  // layers
			8, // neurons per hidden layer
		},
	};
}

}
