#ifndef MY_THREAD_POOL_H
#define MY_THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
// this code from internet : https://blog.csdn.net/MOU_IT/article/details/88712090
class MyThreadPool {

public:
	MyThreadPool(size_t threads=1);
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>;
	~MyThreadPool();
    void set_max_thread_num(int maxThreadNum);
    void Stop();

private:
	std::vector< std::thread > workers;
	std::queue< std::function<void()> > tasks;

	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};

template<class F, class... Args>
auto MyThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;
	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		if (stop)
			throw std::runtime_error("enqueue on stopped MyThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	condition.notify_one();
	return res;
}

#endif