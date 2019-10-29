/**
 * All code related to multithreading
 */
#pragma once

#include <queue>
#include <thread>
#include <mutex>

#include <Eigen/Dense>

#include "ray.hpp"
#include "scene.hpp"

/**
 * We define a raytrace task as an origin ray that needs
 * to be traced on a scene and a pointer to Vector3f were the resulting 
 * color will be stored
 */
struct raytraceTask
{
  Eigen::Vector3f *result = nullptr;
  Ray origin;

  raytraceTask();
  raytraceTask(Eigen::Vector3f *, Ray &);
};

/**
 * Holds a thread safe queue consising of all raytrace tasks.
 */
class TaskQueue
{
  std::queue<raytraceTask> queue;
  std::mutex m;
  Scene *scene;
public:
  std::size_t totalTasks = 0;
  std::size_t completedTasks = 0;

  void push(const raytraceTask &);
  raytraceTask pop();
  bool isEmpty();

  TaskQueue(Scene *);
};

/**
 * Actively tries to pull a raytrace task from the global queue.
 */
class Worker
{
public:
  TaskQueue *globalQueue;
  Scene *workerScene;
  bool done = false;

  void work();
  void end();
};

class Multithreading
{
  unsigned int num_threads;
  std::vector<Worker> workerPool;
  std::vector<std::thread> threadPool;

public:
  TaskQueue globalQueue;
  Multithreading(Scene *);
  ~Multithreading();
};
