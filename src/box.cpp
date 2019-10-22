#include "box.hpp"
#include <limits>
#include <cmath>

/*
We're assuming the input box is AABB (Axis-aligned bounding box)
post on AABB vs. OBB type bounding box:
    https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
*/
Box::Box(vector<Triangle*> &inputTriangles) : Hitable() {
  std::cout << "Creating new box -> #TRIANGLES =  " << inputTriangles.size() << std::endl;
  //Max number of triangles per box  
  int TRIANGLES_PER_BOX = 16310;

  //Set the min max values of the new box to the min and max values
  float xmax = std::numeric_limits<float>::min(), xmin = std::numeric_limits<float>::max();
  float ymax = std::numeric_limits<float>::min(), ymin = std::numeric_limits<float>::max();
  float zmax = std::numeric_limits<float>::min(), zmin = std::numeric_limits<float>::max();
  
  //Loop over the triangles to calculate the bMin and bMax of this box
  for(auto triangle : inputTriangles){
    if(triangle->getMaxX() > xmax) xmax = triangle->getMaxX();
    if(triangle->getMinX() < xmin) xmin = triangle->getMinX();
    if(triangle->getMaxY() > ymax) ymax = triangle->getMaxY();
    if(triangle->getMinY() < ymin) ymin = triangle->getMinY();
    if(triangle->getMaxZ() > zmax) zmax = triangle->getMaxZ();
    if(triangle->getMinZ() < zmin) zmin = triangle->getMinZ();
  }  
  this->bMin = Eigen::Vector3f(xmin, ymin, zmin);
  this->bMax = Eigen::Vector3f(xmax, ymax, zmax);

  //If this box contains more triangles then allowed, split it further
  if ( inputTriangles.size() > TRIANGLES_PER_BOX ) {
    this->splitBox(inputTriangles);  
    this->isLeaf = false;
  } else {
    //At this point we now this box is going to be a leaf box, so add all traingles to to this box
    for(auto triangle : inputTriangles) this->children.push_back( dynamic_cast<Hitable*>(triangle) );
    this->isLeaf = true;
  }
}

void Box::splitBox(vector<Triangle*> &inputTriangles) {
    std::cout << "Box contains to many triangles, so we are going to split" << std::endl;
    float width  = std::abs(this->bMin(0) - this->bMax(0));
    float height = std::abs(this->bMin(1) - this->bMax(1));
    float depth  = std::abs(this->bMin(2) - this->bMax(2));
    int dimensionToSplitOn = width > height ? (width > depth ? 0 : 2) : (height > depth ? 1 : 2);
    std::cout << "dimensionToSplitOn = " << dimensionToSplitOn << std::endl;
    float middlePoint = (this->bMin(dimensionToSplitOn) + this->bMax(dimensionToSplitOn)) / 2 + 0.1;
    std::cout << "low = " << this->bMin(dimensionToSplitOn) << " high = " << this->bMax(dimensionToSplitOn) << " middle = " << middlePoint << std::endl;
    std::vector<Triangle*> left, right;
    for ( Triangle* triangle : inputTriangles ) {
        if ( triangle -> getPosition()(dimensionToSplitOn) < middlePoint ) left.push_back( triangle );
        else right.push_back( triangle );
    }
    std::cout << "#left = " << left.size() << std::endl;
    std::cout << "#right = " << right.size() << std::endl; 
    Hitable* leftBox = dynamic_cast<Hitable*>(new Box(left));
    Hitable* rightBox = dynamic_cast<Hitable*>(new Box(right));
    this->children.push_back(leftBox);
    this->children.push_back(rightBox);
}

// Determine if incoming ray hit a box
bool Box::intersect(float &hitPoint, Ray &ray)
{
    // Lec 8 - Slides 46
    
    // substitute ray in all planes to calculate intersection parameters
    float x_min = this->bMin[0];
    float y_min = this->bMin[1];
    float z_min = this->bMin[2];

    float x_max = this->bMax[0];
    float y_max = this->bMax[1];
    float z_max = this->bMax[2];

    float tx_min = (x_min - ray.origin(0)) / ray.direction(0);
    float ty_min = (y_min - ray.origin(1)) / ray.direction(1);
    float tz_min = (z_min - ray.origin(2)) / ray.direction(2);

    float tx_max = (x_max - ray.origin(0)) / ray.direction(0);
    float ty_max = (y_max - ray.origin(1)) / ray.direction(1);
    float tz_max = (z_max - ray.origin(2)) / ray.direction(2);

    // sort to find in and out
    float t_in_x  = std::min(tx_min, tx_max);
    float t_out_x = std::max(tx_min, tx_max);

    float t_in_y  = std::min(ty_min, ty_max);
    float t_out_y = std::max(ty_min, ty_max);

    float t_in_z  = std::min(ty_min, ty_max);
    float t_out_z = std::max(ty_min, ty_max);

    // determine when we crossed all _in_ points & at least one _out_ point
    float t_in  = std::max(std::max(t_in_x, t_in_y),t_in_z);
    float t_out = std::min(std::min(t_out_x, t_out_y),t_out_z);

    // check for intersection
    if (t_in > t_out || t_out < 0)
        return false;
    return true;
}

std::vector<Box*> Box::getLeafBoxes() {
    std::vector<Box*> leafBoxes;
    if ( this->isLeaf ) leafBoxes.push_back(this);
    else {
        for ( Hitable* child : this->children ) {
            Box* subBox = dynamic_cast<Box*>(child);
            std::vector<Box*> leafBoxesOfSubBox = subBox -> getLeafBoxes();
            leafBoxes.insert( leafBoxes.end(), leafBoxesOfSubBox.begin(), leafBoxesOfSubBox.end() );
        }
    }
    return leafBoxes;
}
