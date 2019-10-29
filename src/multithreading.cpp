#include "multithreading.hpp"

raytraceTask::raytraceTask()
{}

raytraceTask::raytraceTask(Eigen::Vector3f *res, Ray &orig)
{
  this->result = res;
  this->origin = orig;
}

void Worker::work()
{
  while (!this->done)
  {
    if (!globalQueue->isEmpty())
    {
      raytraceTask cur = globalQueue->pop();
      if (cur.result == nullptr)
        continue; //We didn't actually get it, the queue was empty
      this->workerScene->traceRay(cur.result, cur.origin, 0, NULL);
    }
  }
}

void Worker::end()
{
  done = true;
}

void TaskQueue::push(const raytraceTask &task)
{
  std::lock_guard<std::mutex> lock(m);
  queue.push(task);
  totalTasks++;
}

raytraceTask TaskQueue::pop()
{
  std::lock_guard<std::mutex> lock(m);
  if (queue.empty())
    return raytraceTask();
  raytraceTask ret = queue.front();
  queue.pop();
  completedTasks++;
  return ret;
}

bool TaskQueue::isEmpty()
{
  std::lock_guard<std::mutex> lock(m);
  return queue.empty();
}

TaskQueue::TaskQueue(Scene *sc)
{
  this->scene = sc;
}

Multithreading::Multithreading(Scene *sc)
  : globalQueue(sc)
{
  this->num_threads = std::thread::hardware_concurrency();
  workerPool.resize(num_threads);
  threadPool.resize(num_threads);
  for (size_t i = 0; i < num_threads; ++i)
  {
    workerPool[i].globalQueue = &globalQueue;
    workerPool[i].workerScene = sc;
    threadPool[i] = std::thread(&Worker::work, std::ref(workerPool[i]));
  }
}

Multithreading::~Multithreading()
{
  for (size_t i = 0; i < num_threads; ++i)
  {
    workerPool[i].end();
    threadPool[i].join();
  }
}
