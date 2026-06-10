#pragma once

#include <zfs/zfsapi.h>

#include <vector>

namespace zfs {
    namespace math {


        class PICPoint
        {
        public:
            int x;
            int y;

            PICPoint() { x = y = 0; }
            PICPoint(int _x, int _y) { x = _x; y = _y; }
        };


        typedef std::vector <PICPoint> picPtlist;
        typedef std::vector <PICPoint>::iterator picPtlist_it;

        static inline double dotp(PICPoint a, PICPoint b)
        {
            return (double(a.x) * b.x + double(a.y) * b.y);
        }

        static inline bool isPointInTriangleFast(int x, int y, picPtlist& list)
        {
            if (list.size() >= 3)
            {
                //compute vectors
                PICPoint v0(list[2].x - list[0].x, list[2].y - list[0].y);
                PICPoint v1(list[1].x - list[0].x, list[1].y - list[0].y);
                PICPoint v2(x - list[0].x, y - list[0].y);
                //compute dot products
                double dot00 = dotp(v0, v0);
                double dot01 = dotp(v0, v1);
                double dot02 = dotp(v0, v2);
                double dot11 = dotp(v1, v1);
                double dot12 = dotp(v1, v2);
                //compute barycentric coordinates
                double invDenom = 1. / (dot00 * dot11 - dot01 * dot01);
                double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
                double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
                //check if point is in triangle
                return ((u >= 0) && (v >= 0) && ((u + v) < 1)) ? true : false;
            }
            return false;
        }

        static inline bool checkTriangle(int actu, int actv, int imgSz_u, int imgSz_v, double picthr)
        {
            PICPoint p1(0, (int)(picthr * imgSz_v));
            PICPoint p13_1((int)(0.25 * imgSz_u), imgSz_v);
            PICPoint p2(imgSz_u, (int)(picthr * imgSz_v));
            double m = (double(p13_1.y) - p1.y) / (double(p13_1.x) - p1.x);
            PICPoint p3;
            p3.x = (int)(0.5 * imgSz_u);
            p3.y = (int)(m * p3.x + p1.y); //in f(x)
            std::vector <PICPoint> ptTrianglelist;
            ptTrianglelist.push_back(p1);
            ptTrianglelist.push_back(p2);
            ptTrianglelist.push_back(p3);
            return isPointInTriangleFast(actu, actv, ptTrianglelist);
        }

    } // end namespace math
} // end namespace zfs