#pragma once

#include <functional>
#include <future>
#include <string>
#include <atomic>

namespace zf
{

	class AsyncWork
	{
		std::future<int> m_workerResult{};
		std::atomic<bool> m_stopFlag{};

	public:

		typedef std::function< int() > run_f;

		AsyncWork( run_f r )
		{
			m_workerResult = std::async(std::launch::async, std::move(r) );
		}

		void set_stop_flag()
		{
			m_stopFlag = true;
		}

		int wait_thread_stopped()
		{
			return m_workerResult.get();
		}

		int stop_and_wait()
		{
			set_stop_flag();
			return wait_thread_stopped();
		}
	};

}

