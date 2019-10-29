#include "triangleList.hpp"

Hitable *TriangleList::intersect(float &hitPoint, Ray &ray, Hitable *exclude)
{
  Hitable *hitObject;

  for (Triangle *t : this->triangleList)
    if(t->intersect(hitPoint, ray, exclude))
      hitObject = t;

  return hitObject;
}
  
TriangleList::TriangleList(const std::vector<Triangle*> &tl)
{
  triangleList = tl;
}

TriangleList::~TriangleList()
{
  for (Triangle *t : this->triangleList)
    delete t;
}
