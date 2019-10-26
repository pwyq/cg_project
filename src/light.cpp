#include <random>
#include <cstdlib>

#include "light.hpp"

/****************************************************************
 * Light Class Implementation                                   *
 ****************************************************************/

// Constructor
Light::Light(Eigen::Vector3f spectrum, Eigen::Vector3f position) {
	this->spectrum = spectrum;
	this->position = position;
}

/* End of File */