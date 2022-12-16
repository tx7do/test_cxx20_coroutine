#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>
#include <sstream>
#include <cassert>

void accumulate(std::vector<int>::iterator first, std::vector<int>::iterator last, std::promise<int> accumulate_promise)
{
	int sum = std::accumulate(first, last, 0);
	accumulate_promise.set_value(sum);  // Notify future
}

void test1()
{
	// Demonstrate using promise<int> to transmit a result between threads.
	std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
	std::promise<int> accumulate_promise;
	std::future<int> accumulate_future = accumulate_promise.get_future();
	std::thread work_thread(accumulate, numbers.begin(), numbers.end(), std::move(accumulate_promise));

	// future::get() will wait until the future has a valid result and retrieves it.
	// Calling wait() before get() is not needed
	//accumulate_future.wait();  // wait for result
	std::cout << "result=" << accumulate_future.get() << '\n';
	work_thread.join();  // wait for thread completion
}

void do_work(std::promise<void> barrier)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	barrier.set_value();
}

void test2()
{
	// Demonstrate using promise<void> to signal state between threads.
	std::promise<void> barrier;
	std::future<void> barrier_future = barrier.get_future();
	std::thread new_work_thread(do_work, std::move(barrier));
	barrier_future.wait();
	new_work_thread.join();
}

void test3()
{
	std::promise<int> p;
	std::future<int> f = p.get_future();

	p.set_value(42);
	std::cout << f.get() << std::endl;
}

void test4()
{
	std::promise<int> p;
	std::future<int> f = p.get_future();

	std::thread t(
		[](std::promise<int> p)
		{
			p.set_value(42);
		},
		std::move(p));

	std::cout << f.get() << std::endl;
	t.join();
}

void test5()
{
	std::promise<int> p;
	std::future<int> f = p.get_future();

	std::thread t(
		[](std::promise<int> p)
		{
			p.set_value(42);

			try
			{
				p.set_value(43);  // set the second value
			}
			catch (std::future_error& e)
			{
				std::cerr << "caught: " << e.what() << std::endl;
			}
		},
		std::move(p));

	std::cout << f.get() << std::endl;

	try
	{
		std::cout << f.get() << std::endl;  // get the second value
	}
	catch (std::future_error& e)
	{
		std::cerr << "caught: " << e.what() << std::endl;
	}

	t.join();
}

void test6()
{
	std::promise<int> p;
	std::future<int> f = p.get_future();

	std::thread t(
		[](std::promise<int> p)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
			p.set_value(42);
		},
		std::move(p));

	for (int i = 0;; ++i)
	{
		std::cout << "waiting attempt " << i << " ..." << std::endl;
		std::future_status status = f.wait_for(std::chrono::seconds(1));
		if (status != std::future_status::timeout)
		{
			break;
		}
	}
	std::cout << f.get() << std::endl;

	t.join();
}

void test7()
{
	std::vector<long long int> vec;

	std::promise<void> p;

	std::thread t(
		[&vec](std::future<void> f)
		{
			std::cout << "thread: started\n" << std::flush;
			f.wait();
			std::cout << "thread: start computation\n" << std::flush;
			long long int sum = std::accumulate(vec.begin(), vec.end(), 0LL);
			std::cout << "thread: end computation\n" << std::flush;
			std::cout << "sum=" << sum << std::endl;
		},
		p.get_future());

	// Initialize the data
	for (long long int i = 0; i < 1000000; ++i)
	{
		vec.push_back(i);
	}

	std::cout << "main: notify thread\n" << std::flush;
	p.set_value();

	t.join();
}

void test8()
{
	std::promise<int> p;
	std::future<int> f = p.get_future();

	std::thread t(
		[](std::promise<int> p)
		{
			try
			{
				throw std::runtime_error("some exception");
			}
			catch (...)
			{
				p.set_exception(std::current_exception());
			}
		},
		std::move(p));

	try
	{
		std::cout << f.get() << std::endl;
	}
	catch (std::runtime_error& exp)
	{
		std::cout << "main thread: caught: " << exp.what() << std::endl;
	}

	t.join();
}

void test9()
{
	std::promise<std::unique_ptr<int>> p;
	std::future<std::unique_ptr<int>> f = p.get_future();

	std::thread t1(
		[&f]()
		{
			std::cout << "t1: waiting\n" << std::flush;
			int value = *f.get();  // Race condition

			std::ostringstream ss;
			ss << "t1: " << value << "\n";
			std::cout << ss.str() << std::flush;
		});
	std::thread t2(
		[&f]()
		{
			std::cout << "t2: waiting\n" << std::flush;
			int value = *f.get();  // Race condition

			std::ostringstream ss;
			ss << "t2: " << value << "\n";
			std::cout << ss.str() << std::flush;
		});

	std::this_thread::sleep_for(std::chrono::seconds(1));
	p.set_value(std::make_unique<int>(42));

	t1.join();
	t2.join();
}

void test10()
{
	std::promise<std::unique_ptr<int>> p;
	std::future<std::unique_ptr<int>> f = p.get_future();
	std::shared_future<std::unique_ptr<int>> sf = f.share();  // Added

	std::thread t1(
		[sf]()
		{
			// Copy sf by value
			std::cout << "t1: waiting\n" << std::flush;
			int value = *sf.get();

			std::ostringstream ss;
			ss << "t1: " << value << "\n";
			std::cout << ss.str() << std::flush;
		});
	std::thread t2(
		[sf]()
		{
			// Copy sf by value
			std::cout << "t2: waiting\n" << std::flush;
			int value = *sf.get();

			std::ostringstream ss;
			ss << "t2: " << value << "\n";
			std::cout << ss.str() << std::flush;
		});

	std::this_thread::sleep_for(std::chrono::seconds(1));
	p.set_value(std::make_unique<int>(42));

	t1.join();
	t2.join();
}

template<typename Func>
class my_packaged_task;

template<typename Ret, typename... Args>
class my_packaged_task<Ret(Args...)>
{
private:
	std::promise<Ret> promise_;
	std::function<Ret(Args...)> func_;

public:
	my_packaged_task(std::function<Ret(Args...)> func)
		: func_(std::move(func))
	{
	}

	void operator()(Args&& ... args)
	{
		try
		{
			promise_.set_value(func_(std::forward<Args&&>(args)...));
		}
		catch (...)
		{
			promise_.set_exception(std::current_exception());
		}
	}

	std::future<Ret> get_future()
	{
		return promise_.get_future();
	}
};

int compute(int a, int b)
{
	return 42 + a + b;
}

void test11()
{
	std::packaged_task<int(int, int)> task(compute);
	std::future<int> f = task.get_future();
	task(3, 4);
	std::cout << f.get() << std::endl;
}

void test12()
{
	std::packaged_task<int(int, int)> task(compute);
	std::future<int> f = task.get_future();

	std::thread t(std::move(task), 3, 4);  // Added
	t.detach();                            // Added

	std::cout << f.get() << std::endl;
}

void test13()
{
	std::future<int> f = std::async(std::launch::async, compute, 3, 4);
	std::cout << f.get() << std::endl;
}

template<typename Func, typename... Args>
std::future<typename std::result_of<Func(Args...)>::type>
my_async(std::launch policy, Func&& func, Args&& ... args)
{
	assert(policy == std::launch::async && "only async is supported");

	using Result = typename std::result_of<Func(Args...)>::type;
	std::packaged_task<Result(Args...)> task(func);
	std::future<Result> future = task.get_future();
	std::thread t(std::move(task), args...);
	t.detach();
	return future;
}

void test14()
{
	std::future<int> f = std::async(std::launch::deferred, compute, 3, 4);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "this must be the first line\n" << std::flush;
	std::cout << f.get() << std::endl;
}

int main()
{
	test1();
	test2();
	test3();
	test4();
}
