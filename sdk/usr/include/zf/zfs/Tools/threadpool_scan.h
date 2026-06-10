#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <functional>

#include "../linebyline.h"
#include "../zfscan.h"
#include "../clone.h"

namespace zf
{
	/**
	\brief thread pool for processing function in parallel
	*/
    class ThreadPoolScan
    {
    public:
        ThreadPoolScan(
            int nn,             ///< items to process
            int threads_,       ///< threads to use. Maybe use std::thread::hardware_concurrency
            ZFSLinebyline* lbl, ///< the open scan
            std::function<void(ZFSLinebyline* clone, int from, int to, int thidx)> func,
            int n0= 0             ///< start item number (optional)
        )
        :threads(threads_)
        {
            if (nn < 1) return;
            if (!lbl) return;

            if (nn < threads) // if to less items, use just one thread
                threads = nn;
            threads = (std::max)(threads, 1);
            const int step = nn / threads;

            std::vector<std::thread> grp;
            grp.reserve(threads);
            int from = n0;
            int to = from+ step; // (upper border not included in calculations)
            for (int t = 0; t < threads; ++t)
            {
                if (t == threads - 1)
                    to = n0+ nn;

                grp.push_back( 
                    std::thread( [&](int tfrom, int tto, int tidx)
                    {
                    
                        std::unique_ptr<ZFSLinebyline> clone(lbl->Clone());
                    
                        if (clone)
                        {
                            clone->SkipTo(tfrom);
                            clone->Set(0); // set pixel pointer to first pixel to allow L() work fine
                            try
                            {
                                func(clone.get(),tfrom, tto,tidx);
                            }
                            catch (...) {}
                        }
                    } , from, to, t)
                );

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
	
    /**
    \brief thread pool for processing function in parallel
    */
    class ThreadPoolZFScan {
    public:
        ThreadPoolZFScan(
            int nn,             ///< items to process
            int threads_,       ///< threads to use. Maybe use std::thread::hardware_concurrency
            ZFScan* scan,       ///< the scan
            std::function<void(ZFScan* clone, int from, int to, int thidx)> func
        )
            :threads(threads_) {
            if (nn < 1) return;
            if (!scan) return;

            const bool canClone = zfs::cloneSupported(scan);
            if( canClone==false )
                threads = 1;

            if (nn < threads) // if to less items, use just one thread
                threads = nn;
            threads = (std::max)(threads, 1);
            const int step = nn / threads;

            std::vector<std::thread> grp;
            grp.reserve(threads);
            int from = 0;
            int to = step;
            for (int t = 0; t < threads; ++t) {
                if (t == threads - 1)
                    to = nn;
                grp.push_back(std::thread(
                    [&](int tfrom, int tto, int tidx) 
                    {
                        if (canClone)
                        {
                            std::unique_ptr<ZFScan> clone(zfs::ZFSClone(scan));
                            if (clone)
                            {
                                // clone->SkipTo(tfrom);
                                try {
                                    func(clone.get(), tfrom, tto, tidx);
                                }
                                catch (...) {}
                            }
                        }
                        else
                        {
                            try {
                                func(scan, tfrom, tto, tidx);
                            }
                            catch (...) {}
                        }
                    }, from, to, t)
                );
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