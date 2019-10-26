#pragma once

// Tucano framework
#include <tucano/mesh.hpp>
#include <tucano/utils/mtlIO.hpp>

// General C++ library
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>

// Local header files
#include "ray.hpp"
#include "light.hpp"
#include "box.hpp"

class Scene {
public:
    /* Constructor */
    Scene(Tucano::Mesh &, std::vector<Tucano::Material::Mtl> &);
    Scene();

    /* Member Variable */
	Eigen::Vector3f cameraPosition;
	Box* boxOverAllTriangles;

	std::vector<Triangle*> trianglesInScene;
	std::vector<Light*> lights;
	std::vector<Tucano::Material::Mtl>* materials;

	bool useAcc = true;
	bool useThreads = true;

    /* Methods */
	//Will return a color
	void traceRay(Eigen::Vector3f *, Ray &, int, Hitable* exclude);
	void traceRayWithAcc(Eigen::Vector3f *color, Ray &ray, int level, Hitable* exclude);

	//Will return a color
	Eigen::Vector3f shade(Hitable *hitObject, Ray &, float t, int level);

	//Will return a color
	Eigen::Vector3f computeDirectLight(Hitable *hitObject, Eigen::Vector3f hitPoint);
    Eigen::Vector3f computeReflectedLight(Hitable *hitObject, Ray &ray, float t, int level);
};


/****************************************************************
 * Multi-threads                                                *
 ****************************************************************/


struct raytraceTask
{
    Eigen::Vector3f *result = nullptr;
    Ray origin;
	raytraceTask() {}

	raytraceTask(Eigen::Vector3f *r, Ray &o)
	{
	    result = r;
	    origin = o;
	}
};


class TaskQueue
{
    std::queue<raytraceTask> queue;
    std::mutex m;
public:
    std::size_t totalTasks = 0;
    std::size_t completedTasks = 0;

    void push(const raytraceTask &task);
    raytraceTask pop();
    bool isEmpty();
};


class Worker
{
public:
    TaskQueue *globalQueue;
    Scene *workerScene;
    bool done = false;

    void work();
    void end();
};


/* End of File */