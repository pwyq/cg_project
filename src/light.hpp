#pragma once

class Light {
public:
	Eigen::Vector3f spectrum;
	Eigen::Vector3f position;

	Light(Eigen::Vector3f, Eigen::Vector3f);
};
