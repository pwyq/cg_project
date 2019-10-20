#pragma once

#include <vector>
#include "primitives.hpp"

/**
 * @brief Holds data for the entire scene.
 */
//TODO
//Finish naive, better yet implement accelerated
class Scene
{
public:
	std::vector<Hittable> objects;
};
