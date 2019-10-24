#pragma once

#include <Eigen/Dense>
#include <vector>

class Light {
public:
	Eigen::Vector3f spectrum;
	Eigen::Vector3f position;

	Light(Eigen::Vector3f, Eigen::Vector3f);
    /*
    static const int LIGHT_SEED = 42;   // random seed


    std::vector<std::vector<Eigen::Vector3f>> sphericalLights;
    void sphericalLightOn();
    void sphericalLightOff();
    */
};
