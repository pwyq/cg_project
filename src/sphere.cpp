#include "sphere.hpp"

bool solve_quadratic(const float &a, const float &b, const float &c,
                     float &x0, float &x1)
{
  float discr = b * b - 4 * a * c;
  if (discr < 0)
    return false;
  else if (discr == 0)
    x0 = x1 = -0.5 * b / a;
  else {
    float q = (b > 0) ? -0.5 * (b + std::sqrt(discr)) : -0.5 * (b - std::sqrt(discr));
    x0 = q / a;
    x1 = c / q;
  }
  return true;
}

/* TODO: change return type to Hitable
bool Sphere::intersect(float &hitPoint, Ray &ray)
{
    // all variable names refer to:
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection

    float t0, t1;
#if 0
    // geometric solution
    Eigen::Vector3f L = this.center - ray.origin;
    float tca = L.dot(ray.direction);
    float d_squred  = L.dot(L) - tca * tca;
    float radius_squared = std::pow(this.radius, 2);
    if (d_squred > radius_squared)
        return false;
    float thc = std::sqrt(radius_squared - d_squred);

    t0 = tca - thc;
    t1 = tca + thc;
#else
    Eigen::Vector3f L = ray.origin - this->center;
    float a = ray.direction.dot(ray.direction);
    float b = 2 * ray.direction.dot(L);
    float radius_squared = std::pow(this->radius, 2);
    float c = L.dot(L) - radius_squared;
    if (solve_quadratic(a,b,c,t0,t1) == false)
        return false;
#endif

    if (t0 > t1)
        std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1;    // root 0 is -ve
        if (t0 < 0)
            return false;   // both root are -ve
    }
    // t = t0;
    return true;
}
*/
