#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <functional>

namespace zf
{
	/**
	\brief thread pool for processing function in parallel
	*/
    class ThreadPool
    {
    public:
        ThreadPool(
				int nn, 	    ///< items to process
				int threads_,   ///< threads to use
				std::function<void(int from, int to)> func ///< the thread function
        ) 
        :threads(threads_)
        {
            if (nn < 1) return;
            if (nn < threads)
                threads = nn;
            threads = (std::max)(threads, 1);
            const int step = nn / threads;

            std::vector<std::thread> grp;
            grp.reserve(threads);
            int from = 0;
            int to = step;
            for (int t = 0; t < threads; ++t)
            {
                if (t == threads - 1)
                    to = nn;
                grp.push_back( std::thread(
                    [&](int tfrom, int tto)
                {
                    try
                    {
                        func(tfrom, tto);
                    }
                    catch (...) {}
                }
                , from, to));
                from += step;
                to += step;
            }

            // join all
            for (auto& thread : grp)
                if (thread.joinable())
                    thread.join();
        }

        int threads = 1;
    };
	
} // end namespace zf