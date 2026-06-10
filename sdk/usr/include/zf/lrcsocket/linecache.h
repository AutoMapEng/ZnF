#pragma once
/*
    Copyright (C) 2018 by Zoller & Froehlich Gmbh Germany
*/
#include "lrcsocketapi.h"

#ifdef _WINDOWS
#pragma warning(disable:4251)
#endif


#include <vector>
#include <memory>
#include <atomic>
#include <cstring>

namespace lrcsocket
{
    /**
    \brief stores n buffsers
    */
    class LRCSOCKET_API LineCache
    {
    public:
        LineCache(int lines);

        /// pusch back. return false when memory is empty (overrun)
        bool push(uint32_t id, uint32_t typ, uint32_t serie, const char* line, uint32_t lineSize, uint32_t lhsz, uint32_t pixel, uint32_t bpp );
        bool push(uint32_t typ, uint32_t pixel, const char* buf1, uint32_t buf1Size, const char* buf2, uint32_t buf2Size);

        // the memory of each item
        struct LineMem
        {
            enum { 
                T_UNDEF, 
                T_ZH,   ///< zfs-header
                T_LINE,  ///< zfs line with line header
                T_PDATA  ///< already calculated 3d pixel
            };
            uint32_t typ = T_UNDEF;
            uint32_t id = 0;
            uint32_t serie = 0;
            char* ptr = nullptr;
            uint32_t size = 0;
            uint32_t lhsz = 0;
            uint32_t pixel = 0;
            uint32_t bpp = 0; // bytes per pixel
            // if entry has more parts
            uint32_t part1Size{};
            uint32_t part2Size{};

            LineMem() {}
            
            virtual ~LineMem() 
            { 
                delete [] ptr; 
            }

            // resize memory of line
            void resize(uint32_t sz)
            {
                if (sz && size != sz)
                {
                    delete [] ptr;
                    ptr = new char[sz];
                    size = sz;
#ifdef _DEBUG
                    memset(ptr, 0x66, sz);
#endif
                }
            }

            void copy( const char* buf, uint32_t sz )
            {
                if( sz && ptr && sz==size )
                    memcpy(ptr, buf, sz);
                part1Size = sz;
                part2Size = 0;
            }
            void copy(const char* buf1, uint32_t sz1, const char* buf2, uint32_t sz2)
            {
                if (sz1 && ptr && (sz1 + sz2) == size)
                {
                    memcpy(ptr, buf1, sz1);
                    memcpy(ptr+sz1, buf2, sz2);
                    part1Size = sz1;
                    part2Size = sz2;
                }
            }
        };

        /// pop line from cache. return false if there is nothing in the cache
        bool pop();

        const LineMem* get(uint32_t offset ) const;
        const LineMem* mem(uint32_t idx) const;

        /// the size of the cache
        int size() const { return cacheSize; }

        /// counts every add (push)
        int added() const { return nAdded; }
        
        /// is there anything in the cache
        int available() const;

        
        int inpos() const { return in; }
        
        int outpos() const { return out; }

    protected:
        std::vector< LineMem > cache;
        std::atomic<int> in {0};
        std::atomic<int> out {0};
        int cacheSize = 0;
        int nAdded = 0;
    };
}
