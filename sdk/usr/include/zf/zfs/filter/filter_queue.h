#pragma once
#include "../zfsapi.h"

#include <functional>
#include <string>
#include <vector>

#include "../pixel.h"



class ZFScan;
class ZFSLinebyline;

namespace zfs
{
	/*
	* A queue for pixel data PDatas
	* 
	* Noch buggy
	* 
	*/
	class PDatasQueue
	{
	public:

		PDatasQueue(int queueSize, int pixel_=0)
		{
			buf.resize(queueSize);
			setPixelSize(pixel_);
		}

		void setPixelSize(int pixel_)
		{
			if (pixel != pixel_ )
			{
				pixel = pixel_;
				for (auto& b : buf)
					b.points.resize(pixel);
			}
		}

		typedef std::function< void(PDatasQueue*, int) > act_cb_f;
		void act(act_cb_f f)
		{
			a = f;
		}

		typedef std::function< void(zfs::PDatas<zfs::PDataDetail>* bufToFillup) > fill_cb_f;
        void fill(int line,  int thid, fill_cb_f f)
        {
            // rotate
            buf[0] = buf[1];
            buf[1] = buf[2];
            // fill at end
			buf[2].clr();
			// fillup with new line
			auto& b = buf[buf.size() - 1];
            f( &b );
			b.line = line;
			b.valid = true;

            cur++;
            if (cur >= static_cast<int>(buf.size()))
            {
				// if queue filled -> call action (filter)
				if (a) a(this, 3);

                // output the first in the queue to have space for next fillup
                if (cb) cb(&buf[0],context,thid);
            }
			else
			{
				// if queue filled -> call action (filter)
				if( buf[1].valid )
					if (a) a(this, 1);
			}
		}


		typedef std::function< void(const zfs::PDatas<zfs::PDataDetail>*, void* context, int thid ) > data_cb_f;
		void out(data_cb_f c)
		{
			cb = c;
		}

		void end( int thid )
		{
			if (cb)
			{
				buf[0] = buf[1];
				buf[1] = buf[2];
				if (a) a(this, 1);
				if( buf[0].valid )
					cb(&buf[0],context,thid);

				buf[0] = buf[1];
				buf[1] = buf[2];
				if (a) a(this, 1);
				if (buf[0].valid)
					cb(&buf[0], context,thid);
			}
		}

		void stop()
		{
			bStop = true;
		}

		int pixel{}; ///< number of pixel each line
		std::vector< zfs::PDatas<zfs::PDataDetail> > buf;
		int cur{}; ///< current
		data_cb_f cb{}; ///< out callback
		act_cb_f a{}; ///< additional action callback
		void* context{};
		bool bStop{ false };
	};



} // end namespace
