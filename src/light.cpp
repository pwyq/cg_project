#include "light.hpp"

Light::Light(Eigen::Vector3f spectrum, Eigen::Vector3f position) {
	this->spectrum = spectrum;
	this->position = position;
}