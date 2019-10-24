#include <random>

#include "light.hpp"


Light::Light(Eigen::Vector3f spectrum, Eigen::Vector3f position) {
	this->spectrum = spectrum;
	this->position = position;
}


void Light::clearSphericalLights() {
    if (sphericalLightsPos.size() > 0)
        this->sphericalLightsPos.clear();
}


void Light::sphericalLightOn(std::vector<Eigen::Vector3f>& lights) {
    clearSphericalLights();

    // randomness generator
    std::mt19937 seed(this->LIGHT_SEED);
    std::uniform_real_distribution<float> rndFloat(0.0, 1.0);

    Eigen::Vector3f currLightPos = lights.back();// need error checking here?
    lights.pop_back();
    std::cout << "M key is pressed and current light position = " << currLightPos << std::endl;


    // std::vector<Eigen::Vector3f> newLightSphere;

    // if (sphericalCount[sphericalCount.size()-1] > 1 && sphericalRadius[sphericalRadius.size()-1] > 0) {
    //     // https://stackoverflow.com/a/13776158/10620764
    //     float lightIntensity = sphericalIntensity.back();
    //     sphericalIntensity.pop_back();
    //     int lightCount = sphericalCount.back();
    //     float radius = sphericalRadius.back();
    //     for (int i = 0; i < sphericalCount; i++) {
    //         float theta = 2 * M_PI * rndFloat(seed);
    //         float phi = acos(1 - 2 * rndFloat(seed));
    //         float x = currLightPos[0] + std::sin(phi) * std::cos(theta) * radius;
    //         float y = currLightPos[1] + std::sin(phi) * std::cos(theta) * radius;
    //         float z = currLightPos[2] + std::cos(phi) * radius;
    //         Eigen::Vector3f offset = Eigen::Vector3f(x, y, z);
    //         lights.push_back(offset);
    //         sphericalIntensity.push_back(lightIntensity / lightCount);
    //     }
    // }
    // else {
    //     lights.push_back(currLightPos);
    // }
    // sphericalLightsPos.push_back(newLightSphere);
}


void Light::sphericalLightOff() {
    // TODO
}


void Light::allLightsOff() {
    // TODO
}

/* End of File */