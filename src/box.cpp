#include "box.hpp"

// Determine if ray hit a box
bool Box::intersect(float &hitPoint, Ray &ray)
{
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

    float t_in_x  = std::min(tx_min, tx_max);
    float t_out_x = std::max(tx_min, tx_max);

    float t_in_y  = std::min(ty_min, ty_max);
    float t_out_y = std::max(ty_min, ty_max);

    float t_in_z  = std::min(ty_min, ty_max);
    float t_out_z = std::max(ty_min, ty_max);

    float t_in  = std::max(t_in_x, t_in_y, t_in_z);
    float t_out = std::min(t_out_x, t_out_y, t_out_z);

    if (t_in > t_out || t_out < 0)
        return false;
    return true;
}
