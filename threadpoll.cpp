#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include<iostream>
#include <future>

class ThreadPool {
public:
	explicit ThreadPool(size_t threadNum) :stop(false) {
		for (int i = 0; i < threadNum; i++) {
			workers.emplace_back([this] {
				for (;;) {
					std::function<void()>task;
					{
						std::unique_lock<std::mutex> ul(mtx);
						cv.wait(ul, [this] {return stop || !tasks.empty(); });
						if (stop&&tasks.empty())return;
						task = move(tasks.front());
						tasks.pop();
					}
					task();
				}
			});
		}
	}
	~ThreadPool() {
		{
			std::unique_lock<std::mutex>ul(mtx);
			stop = true;
		}
		cv.notify_all();
		for (auto& worker : workers) {
			worker.join();
		}
	}
	template<typename F, typename... Args>
	auto submit(F&& f, Args&&...args)->std::future<decltype(f(args...))>{
		auto taskptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
			std::bind(std::forward<F>(f),std::forward<Args>(args)...)
		);
		{
			std::unique_lock<std::mutex>ul(mtx);
			tasks.emplace([taskptr] { (*taskptr)(); });
		}
		cv.notify_one();
		return taskptr->get_future();
	}
private:
	bool stop;
	std::vector<std::thread>workers;
	std::queue<std::function<void()>>tasks;
	std::mutex mtx;
	std::condition_variable cv;
};
int main() {
	ThreadPool tp(5);
	for (int i = 0; i < 10; i++) {
		tp.submit([i]()->void {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			std::cout << i << std::endl;
			return;
		});
	}
	
	return 0;
}
