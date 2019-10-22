#include "box.hpp"

/*
We're assuming the input box is AABB (Axis-aligned bounding box)
post on AABB vs. OBB type bounding box:
    https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
*/

Box::Box(Eigen::Vector3f x, Eigen::Vector3f y, bool isLeaf): Hitable(){
      this->bMin = x;
        this->bMax = y;
        this->isLeaf = isLeaf;
}

// Determine if incoming ray hit a box
bool Box::intersect(float &hitPoint, Ray &ray)
{
    // Lec 8 - Slides 46
    
    // substitute ray in all planes to calculate intersection parameters
    float x_min = this.bMin[0];
    float y_min = this.bMin[1];
    float z_min = this.bMin[2];

    float x_max = this.bMax[0];
    float y_max = this.bMax[1];
    float z_max = this.bMax[2];

    float tx_min = (x_min - ray.origin.x) / ray.direction.x;
    float ty_min = (y_min - ray.origin.y) / ray.direction.y;
    float tz_min = (z_min - ray.origin.z) / ray.direction.z;

    float tx_max = (x_max - ray.origin.x) / ray.direction.x;
    float ty_max = (y_max - ray.origin.y) / ray.direction.y;
    float tz_max = (z_max - ray.origin.z) / ray.direction.z;

    // sort to find in and out
    float t_in_x  = std::min(tx_min, tx_max);
    float t_out_x = std::max(tx_min, tx_max);

    float t_in_y  = std::min(ty_min, ty_max);
    float t_out_y = std::max(ty_min, ty_max);

    float t_in_z  = std::min(ty_min, ty_max);
    float t_out_z = std::max(ty_min, ty_max);

    // determine when we crossed all _in_ points & at least one _out_ point
    float t_in  = std::max(t_in_x, t_in_y, t_in_z);
    float t_out = std::min(t_out_x, t_out_y, t_out_z);

    // check for intersection
    if (t_in > t_out || t_out < 0)
        return false;
    return true;
}
