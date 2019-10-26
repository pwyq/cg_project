#pragma once

#include "light.hpp"

class SphereLight : public Light {
public:
    float sphericalRadius; //radius of sphere
    int sphericalCount;    //number of point lights in sphere
    Eigen::Vector3f intensity;   //intensity of each point light (total intensity of sphere / #point lights)
    std::vector<Eigen::Vector3f> sphericalLightsPos;

    /* Methods */
	SphereLight(Eigen::Vector3f spec, Eigen::Vector3f pos);
	void createSphereLight();
	virtual float getVisibility(Eigen::Vector3f) { return 0.0; }
};