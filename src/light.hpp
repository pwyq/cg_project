#pragma once

#include <Eigen/Dense>
#include <vector>
#include <iostream> // remove this later

#include <tucano/camera.hpp>

class Light {
    void allLightsOff();
    unsigned int totalSphereLight = 0;
public:
    /* Constructor */
    // spectrum, position
	Light(Eigen::Vector3f, Eigen::Vector3f);

    /* Member Constants */
    const int LIGHT_SEED = 42;   // hardcoded random seed
    const int COUNT_LIMIT = 50;
    const float INTENSITY_LIMIT = 100.0;
    const float RADIUS_LIMIT = 3.0;

    /* Member Variables */
	Eigen::Vector3f spectrum;
	Eigen::Vector3f position;
    bool isSphericalLightOn = false;

    // Use vector, in case we may need multiple sphere lights
    std::vector<float> sphericalRadius;     // radius of each sphere
    std::vector<int> sphericalCount;        // number of point of each sphere
    std::vector<float> sphericalIntensity;  // intensity of each sphere
    std::vector<std::vector<Eigen::Vector3f>> sphericalLightsPos;

    /* Methods */
    void clearSphericalLights();
    void sphericalLightOn(std::vector<Eigen::Vector3f>& lights);
    void addSphericalLights(std::vector<Eigen::Vector3f>& lights,
                            unsigned int index);
    void sphericalLightOff();
    // void pointLightOff();

    // Getter methods
    unsigned int getTotalSphereLight();

    // Setter methods
    void setTotalSphereLight(unsigned int);
};

/* End of File */