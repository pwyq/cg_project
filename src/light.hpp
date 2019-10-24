#pragma once

#include <Eigen/Dense>
#include <vector>
#include <iostream> // remove this later

class Light {
    void clearSphericalLights();
    void allLightsOff();
public:
    /* Constructor */
    // spectrum, position
	Light(Eigen::Vector3f, Eigen::Vector3f);

    /* Member Constants */
    const int LIGHT_SEED = 42;   // hardcoded random seed

    /* Member Variables */
	Eigen::Vector3f spectrum;
	Eigen::Vector3f position;

    std::vector<float> sphericalRadius;
    std::vector<int> sphericalCount;
    std::vector<float> sphericalIntensity;
    std::vector<std::vector<Eigen::Vector3f>> sphericalLightsPos;

    /* Methods */
    void sphericalLightOn(std::vector<Eigen::Vector3f>& lights);
    void sphericalLightOff();
};

/* End of File */