
#include "population.hpp"

namespace eagereye {

PopulationLayout make_default_population_layout() {
	return {
		100, // size_
		//BrainLayout
		{
		  6,// inputs
			1,  // outputs
			5,  // layers
			6, // neurons per hidden layer
		},
	};
}

}
