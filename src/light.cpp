#include "light.hpp"

Light::Light(Eigen::Vector3f spectrum, Eigen::Vector3f position) {
	this->spectrum = spectrum;
	this->position = position;
}

/*
void Light::sphericalLightOn() {
    this->sphericalLights.clear();

    std::mt19937 seed(LIGHT_SEED);
    std::uniform_real_distribution<float> rndFloat(0.0, 1.0);
}

void Light::sphericalLightOn() {

}
*/

/* End of File */