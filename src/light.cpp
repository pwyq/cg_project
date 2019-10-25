#include <random>
#include <cstdlib>

#include "light.hpp"


Light::Light(Eigen::Vector3f spectrum, Eigen::Vector3f position) {
	this->spectrum = spectrum;
	this->position = position;
}


void Light::clearSphericalLights() {
    if (getTotalSphereLight() > 0) {
        this->sphericalRadius.clear();
        this->sphericalCount.clear();
        this->sphericalIntensity.clear();
        this->sphericalLightsPos.clear();
        setTotalSphereLight(0);
    }
}


// void Light::pointLightOff() {
//     if (lights.size() > 0)
//         lights.clear();
// }

float generateRandomFloat(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

// change to addSphericalLights()
void Light::sphericalLightOn(std::vector<Eigen::Vector3f>& lights) {
    isSphericalLightOn = true;
    std::cout << "[INFO] Then God said, \"Let there be light,\" and there was light.\n";
    // clearSphericalLights();
    // pointLightOff();
    unsigned int index = getTotalSphereLight();
    setTotalSphereLight(index+1);
    std::mt19937 randomGenerator(LIGHT_SEED);
    std::srand(LIGHT_SEED);

    sphericalRadius.push_back(generateRandomFloat(1.0, RADIUS_LIMIT));
    sphericalCount.push_back(std::rand() % COUNT_LIMIT);
    sphericalIntensity.push_back(generateRandomFloat(1.0, INTENSITY_LIMIT) / sphericalCount[index]);

    addSphericalLights(lights, index);
}


unsigned int Light::getTotalSphereLight() {
    return totalSphereLight;
}

void Light::setTotalSphereLight(unsigned int idx) {
    totalSphereLight = idx;
}


void Light::addSphericalLights(std::vector<Eigen::Vector3f>& lights, unsigned int index) {
    Eigen::Vector3f currLightPos = Eigen::Vector3f(0, 0, 0);
    if (lights.size() > 0) {
        currLightPos = lights.back();
        lights.pop_back();
    }

    // http://corysimon.github.io/articles/uniformdistn-on-sphere/
    std::mt19937 randomGenerator(this->LIGHT_SEED);
    std::uniform_real_distribution<float> uniform01(0.0, 1.0);

    std::vector<Eigen::Vector3f> newPointLights;
    for (int i = 0; i < this->sphericalCount[index]; i++) {
        float theta = 2 * M_PI * uniform01(randomGenerator);
        float phi = acos(1 - 2 * uniform01(randomGenerator));
        float x = sin(phi) * cos(theta) * sphericalRadius[index];
        x += currLightPos[0];
        float y = sin(phi) * sin(theta) * sphericalRadius[index];
        y += currLightPos[1];
        float z = cos(phi) * sphericalRadius[index];
        z += currLightPos[2];

        Eigen::Vector3f newPointLight = Eigen::Vector3f(x, y, z);
        newPointLights.push_back(newPointLight);
    }
    this->sphericalLightsPos.push_back(newPointLights);
}


void Light::sphericalLightOff() {
    isSphericalLightOn = false;
    std::cout << "[INFO] Then God said, \"Let there be dark,\" and there was no light.\n";
    // TODO
}


void Light::allLightsOff() {
    // TODO
}

/* End of File */