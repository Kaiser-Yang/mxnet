#include "my_thread_pool.h"

MyThreadPool::MyThreadPool(size_t threads) : stop(false) {
	set_max_thread_num(threads);
}

void MyThreadPool::Stop() {
    {
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& worker : workers) worker.join();
}

void MyThreadPool::set_max_thread_num(int maxThreadNum) {
    Stop();
    stop = false;
	workers.clear();
    while (workers.size() < maxThreadNum) {
        workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

MyThreadPool::~MyThreadPool()
{
    Stop();
}
