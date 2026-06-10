#pragma once

#include <cfloat>
#include <climits>

#include <Common/inttypes_.h>

#include <zfs/Math/math/tuple.h>

#include <zfs/Math/2d/point.h>
#include <zfs/Math/2d/line.h>

namespace zfs {
    namespace math {
        // ===================================================================================================================
#define EPS FLT_EPSILON
// ===================================================================================================================
        template <class T> class QuadEdge;
        // ===================================================================================================================
        /*
        * @brief class for representing an edge
        * @author Dani Lischinski (danix@graphics.cornell.edu)
        * see https://graphics.stanford.edu/courses/cs468-02-fall/readings/lischinski.ps
        */
        template <class T> class Edge
        {
            template<class S> friend class QuadEdge;
            template<class S> friend void Splice(Edge<S>*, Edge<S>*);
        private:
            bool      bHandledForTriangleList;
            int       num;
            Edge* next;
            Point<T>* data;

        public:
            Edge()
            {
                data = 0;
                next = 0;
                bHandledForTriangleList = false;
            }
            ~Edge()
            {
                if (data)
                {
                    delete data;
                    data = 0;
                }
            }
            Edge* Rot();
            Edge* invRot();
            Edge* Sym();
            Edge* Onext();
            Edge* Oprev();
            Edge* Dnext();
            Edge* Dprev();
            Edge* Lnext();
            Edge* Lprev();
            Edge* Rnext();
            Edge* Rprev();
            Point<T>* Org();
            Point<T>* Dest();
            const Point<T>& Org2d() const;
            const Point<T>& Dest2d() const;
            void EndPoints(Point<T>*, Point<T>*);
            QuadEdge<T>* Qedge()
            {
                return (QuadEdge<T> *)(this - num);
            }
        };
        // ===================================================================================================================
        template <class T> class Subdivision;
        // ===================================================================================================================
        /*
        * @brief class for representing a QuadEdge
        * @author Dani Lischinski (danix@graphics.cornell.edu)
        * see https://graphics.stanford.edu/courses/cs468-02-fall/readings/lischinski.ps
        */
        template <class T> class QuadEdge
        {
            template<class S> friend class Subdivision;
        private:
            Edge<T> e[4];
        public:
            QuadEdge();
        };
        // ===================================================================================================================
        /*
        * @brief class for representing a triangle
        * @author Holzner
        */
        class Triangle
        {
            Tuple3<int> m_PtIdcs; ///< point indices belonging to a triangle

        public:
            Triangle() : m_PtIdcs(Tuple3<int>()) {}
            Triangle(const Tuple3<int>& aTuple) : m_PtIdcs(aTuple) {}

            int operator[](const int aIdx) const
            {
                return m_PtIdcs[aIdx];
            }

        };
        // ===================================================================================================================
        /*
        * @brief triangle list type
        * @author Holzner
        */
        typedef std::vector< Triangle> TriangleList_t;
        // ===================================================================================================================
        /*
        * @brief class for representing a Subdivision (= Incremental Delauney)
        * @author Dani Lischinski (danix@graphics.cornell.edu), extensions J. Holzner
        * see https://graphics.stanford.edu/courses/cs468-02-fall/readings/lischinski.ps
        */
        template <class T> class Subdivision
        {


        private:
            Edge<T>* startingEdge;
            Edge<T>* Locate(const Point<T>&);

            std::list< QuadEdge<T>*> m_QuadEdgesUsed; /// < for deleting the generated edges at the end
            TriangleList_t           m_TriangleList;     /// < index reference (modifikation point data to indices)

            void checkLeftSide(Edge<T>* edgeToBeChecked);
            void checkRightSide(Edge<T>* edgeToBeChecked);


        public:
            Subdivision()
            {
                startingEdge = 0;
            }

            Subdivision(const Point<T>&, const Point<T>&, const Point<T>&);

            void init(const Point<T>&, const Point<T>&, const Point<T>&);

            static Edge<T>* MakeEdge(Subdivision<T>* const pSubdivision); /// < for deleting the generated edges at the end moved here
            ~Subdivision();
            void InsertSite(const Point<T>&);

            void AddQuadEdge(QuadEdge<T>* const apQuadEdge); /// < for deleting the generated edges at the end

            TriangleList_t getListOfTriangles();

        };
        // ===================================================================================================================
        // Implementation
        // ===================================================================================================================
        template <class T> inline  QuadEdge<T>::QuadEdge()
        {
            e[0].num = 0, e[1].num = 1, e[2].num = 2, e[3].num = 3;
            e[0].next = &(e[0]); e[1].next = &(e[3]);
            e[2].next = &(e[2]); e[3].next = &(e[1]);
        }
        // ===================================================================================================================
        // Edge Algebra 
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Rot()
            // Return the dual of the current edge, directed from its right to its left.
        {
            return (num < 3) ? this + 1 : this - 3;
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::invRot()
            // Return the dual of the current edge, directed from its left to its right.
        {
            return (num > 0) ? this - 1 : this + 3;
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Sym()
            // Return the edge from the destination to the origin of the current edge.
        {
            return (num < 2) ? this + 2 : this - 2;
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Onext()
            // Return the next ccw edge around (from) the origin of the current edge.
        {
            return next;
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Oprev()
            // Return the next cw edge around (from) the origin of the current edge.
        {
            return Rot()->Onext()->Rot();
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Dnext()
            // Return the next ccw edge around (into) the destination of the current edge.
        {
            return Sym()->Onext()->Sym();
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Dprev()
            // Return the next cw edge around (into) the destination of the current edge.
        {
            return invRot()->Onext()->invRot();
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Lnext()
            // Return the ccw edge around the left face following the current edge.
        {
            return invRot()->Onext()->Rot();
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Lprev()
            // Return the ccw edge around the left face before the current edge.
        {
            return Onext()->Sym();
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Rnext()
            // Return the edge around the right face ccw following the current edge.
        {
            return Rot()->Onext()->invRot();
        }
        // ===================================================================================================================
        template <class T> inline  Edge<T>* Edge<T>::Rprev()
            // Return the edge around the right face ccw before the current edge.
        {
            return Sym()->Onext();
        }
        // ===================================================================================================================
        // Access to data pointers 
        // ===================================================================================================================
        template <class T> inline  Point<T>* Edge<T>::Org()
        {
            return data;
        }
        // ===================================================================================================================
        template <class T> inline  Point<T>* Edge<T>::Dest()
        {
            return Sym()->data;
        }
        // ===================================================================================================================
        template <class T> inline  const Point<T>& Edge<T>::Org2d() const
        {
            return *data;
        }
        // ===================================================================================================================
        template <class T> inline  const Point<T>& Edge<T>::Dest2d() const
        {
            return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
        }
        // ===================================================================================================================
        template <class T> inline  void Edge<T>::EndPoints(Point<T>* a_or, Point<T>* a_de)
        {
            data = a_or;
            Sym()->data = a_de;
        }
        // ===================================================================================================================
        // Basic Topological Operators 
        // ===================================================================================================================
        template <class T> void Subdivision<T>::AddQuadEdge(QuadEdge<T>* const apQuadEdge)
        {
            m_QuadEdgesUsed.push_back(apQuadEdge);
        }
        // ===================================================================================================================
        template <class T> Edge<T>* Subdivision<T>::MakeEdge(Subdivision<T>* const pSubdivision)
        {
            QuadEdge<T>* ql = new QuadEdge<T>;

            pSubdivision->AddQuadEdge(ql);

            return ql->e;
        }
        // ===================================================================================================================
        template <class T> void Splice(Edge<T>* a, Edge<T>* b)
            // This operator affects the two edge rings around the origins of a and b,
            // and, independently, the two edge rings around the left faces of a and b.
            // In each case, (i) if the two rings are distinct, Splice will combine
            // them into one; (ii) if the two are the same ring, Splice will break it
            // into two separate pieces.
            // Thus, Splice can be used both to attach the two edges together, and
            // to break them apart. See Guibas and Stolfi (1985) p.96 for more details
            // and illustrations.
        {
            Edge<T>* alpha = a->Onext()->Rot();
            Edge<T>* beta = b->Onext()->Rot();
            Edge<T>* t1 = b->Onext();
            Edge<T>* t2 = a->Onext();
            Edge<T>* t3 = beta->Onext();
            Edge<T>* t4 = alpha->Onext();
            a->next = t1;
            b->next = t2;
            alpha->next = t3;
            beta->next = t4;
        }
        // ===================================================================================================================
        template <class T> void DeleteEdge(Edge<T>* e)
        {
            Splice(e, e->Oprev());
            Splice(e->Sym(), e->Sym()->Oprev());
            delete e->Qedge();
        }
        // ===================================================================================================================
        // Topological Operations for Delaunay Diagrams 
        // ===================================================================================================================
        template <class T> TriangleList_t Subdivision<T>::getListOfTriangles()
        {
            startingEdge->bHandledForTriangleList = true;

            checkLeftSide(startingEdge);
            checkRightSide(startingEdge);

            return TriangleList_t();
        }
        // ===================================================================================================================
        template <class T> void Subdivision<T>::checkLeftSide(Edge<T>* edgeToBeChecked)
        {
            Edge<T>* e2tl = edgeToBeChecked->Onext(); // ccw, left triangle
            Edge<T>* e3tl = edgeToBeChecked->Lnext();
            // left side
            if (edgeToBeChecked != e2tl && edgeToBeChecked != e3tl)
            {
                // is a triangle -> mutex, add to triangle list if not already in


            }

            if (edgeToBeChecked != e2tl && !e2tl->bHandledForTriangleList)
            {
                e2tl->bHandledForTriangleList = true;
                checkLeftSide(e2tl);
                checkRightSide(e2tl);
            }

            if (!e3tl->bHandledForTriangleList)
            {
                e3tl->bHandledForTriangleList = true;
                checkLeftSide(e3tl);
                checkRightSide(e3tl);
            }

        }
        // ===================================================================================================================
        template <class T> void Subdivision<T>::checkRightSide(Edge<T>* edgeToBeChecked)
        {
            Edge<T>* e2tr = edgeToBeChecked->Dnext(); // ccw, right triangle
            Edge<T>* e3tr = edgeToBeChecked->Rnext();

            // right side
            if (edgeToBeChecked != e2tr && edgeToBeChecked != e3tr)
            {
                // is a triangle
            }
            else
            {

            }
        }


        // ===================================================================================================================
        template <class T> Subdivision<T>::Subdivision(const Point<T>& a, const Point<T>& b, const Point<T>& c)
            // Initialize a subdivision to the triangle defined by the points a, b, c.
        {
            init(a, b, c);
        }
        // ===================================================================================================================
        template <class T> void Subdivision<T>::init(const Point<T>& a, const Point<T>& b, const Point<T>& c)
        {
            Point<T>* da, * db, * dc;
            da = new Point<T>(a), db = new Point<T>(b), dc = new Point<T>(c);
            Edge<T>* ea = MakeEdge(this);
            ea->EndPoints(da, db);
            Edge<T>* eb = MakeEdge(this);
            Splice(ea->Sym(), eb);
            eb->EndPoints(db, dc);
            Edge<T>* ec = MakeEdge(this);
            Splice(eb->Sym(), ec);
            ec->EndPoints(dc, da);
            Splice(ec->Sym(), ea);
            startingEdge = ea;
        }
        // ===================================================================================================================
        template <class T> Subdivision<T>::~Subdivision()
        {
            auto QuadEdge_it = m_QuadEdgesUsed.begin();
            for (; QuadEdge_it != m_QuadEdgesUsed.end(); ++QuadEdge_it)
            {
                if (*QuadEdge_it)
                {
                    delete* QuadEdge_it;
                    *QuadEdge_it = 0;
                }
            }
        }
        // ===================================================================================================================
        template<class T> Edge<T>* Connect(Edge<T>* a, Edge<T>* b, Subdivision<T>* pSubdivision)
            // Add a new edge e connecting the destination of a to the
            // origin of b, in such a way that all three have the same
            // left face after the connection is complete.
            // Additionally, the data pointers of the new edge are set.
        {
            Edge<T>* e = Subdivision<T>::MakeEdge(pSubdivision);
            Splice(e, a->Lnext());
            Splice(e->Sym(), b);
            e->EndPoints(a->Dest(), b->Org());
            return e;
        }
        // ===================================================================================================================
        template<class T> void Swap(Edge<T>* e)
            // Essentially turns edge e counterclockwise inside its enclosing
            // quadrilateral. The data pointers are modified accordingly.
        {
            Edge<T>* a = e->Oprev();
            Edge<T>* b = e->Sym()->Oprev();
            Splice(e, a);
            Splice(e->Sym(), b);
            Splice(e, a->Lnext());
            Splice(e->Sym(), b->Lnext());
            e->EndPoints(a->Dest(), b->Dest());
        }
        // ===================================================================================================================
        // Geometric Predicates for Delaunay Diagrams 
        // ===================================================================================================================
        template<class T> inline  T TriArea(const Point<T>& a, const Point<T>& b, const Point<T>& c)
            // Returns twice the area of the oriented triangle (a, b, c), i.e., the
            // area is positive if the triangle is oriented counterclockwise.
        {
            return (b.getX() - a.getX()) * (c.getY() - a.getY()) - (b.getY() - a.getY()) * (c.getX() - a.getX());
        }
        // ===================================================================================================================
        template <class T> int InCircle(const Point<T>& a, const Point<T>& b,
            const Point<T>& c, const Point<T>& d)
            // Returns true if the point d is inside the circle defined by the
            // points a, b, c. See Guibas and Stolfi (1985) p.107.
        {
            return (a.getX() * a.getX() + a.getY() * a.getY()) * TriArea(b, c, d) -
                (b.getX() * b.getX() + b.getY() * b.getY()) * TriArea(a, c, d) +
                (c.getX() * c.getX() + c.getY() * c.getY()) * TriArea(a, b, d) -
                (d.getX() * d.getX() + d.getY() * d.getY()) * TriArea(a, b, c) > 0;
        }
        // ===================================================================================================================
        template <class T> int ccw(const Point<T>& a, const Point<T>& b, const Point<T>& c)
            // Returns true if the points a, b, c are in a counterclockwise order
        {
            return (TriArea(a, b, c) > 0);
        }
        // ===================================================================================================================
        template <class T> int RightOf(const Point<T>& x, Edge<T>* e)
        {
            return ccw(x, e->Dest2d(), e->Org2d());
        }
        // ===================================================================================================================
        template <class T> int LeftOf(const Point<T>& x, Edge<T>* e)
        {
            return ccw(x, e->Org2d(), e->Dest2d());
        }
        // ===================================================================================================================
        template <class T> int OnEdge(const Point<T>& x, Edge<T>* e)
            // A predicate that determines if the point x is on the edge e.
            // The point is considered on if it is in the EPS-neighborhood
            // of the edge.
        {
            T t1, t2, t3;
            t1 = (T)(x - e->Org2d()).Norm();
            t2 = (T)(x - e->Dest2d()).Norm();
            if (t1 < EPS || t2 < EPS)
                return true;
            t3 = (T)(e->Org2d() - e->Dest2d()).Norm();
            if (t1 > t3 || t2 > t3)
                return false;
            Line<T> line(e->Org2d(), e->Dest2d());
            return line.onLine(x);
        }
        // ===================================================================================================================
        // An Incremental Algorithm for the Construction of Delaunay Diagrams 
        // ===================================================================================================================
        template <class T> Edge<T>* Subdivision<T>::Locate(const Point<T>& x)
            // Returns an edge e, s.t. either x is on e, or e is an edge of
            // a triangle containing x. The search starts from startingEdge
            // and proceeds in the general direction of x. Based on the
            // pseudocode in Guibas and Stolfi (1985) p.121.
        {
            Edge<T>* e = startingEdge;
            while (true)
            {
                if (x == e->Org2d() || x == e->Dest2d())
                    return e;
                else if (RightOf(x, e))
                    e = e->Sym();
                else if (!RightOf(x, e->Onext()))
                    e = e->Onext();
                else if (!RightOf(x, e->Dprev()))
                    e = e->Dprev();
                else
                    return e;
            }
        }
        // ===================================================================================================================
        template <class T> void Subdivision<T>::InsertSite(const Point<T>& x)
            // Inserts a new point into a subdivision representing a Delaunay
            // triangulation, and fixes the affected edges so that the result
            // is still a Delaunay triangulation. This is based on the
            // pseudocode from Guibas and Stolfi (1985) p.120, with slight
            // modifications and a bug fix.
        {
            Edge<T>* e = Locate(x);
            if ((x == e->Org2d()) || (x == e->Dest2d())) // point is already in
                return;
            else if (OnEdge(x, e))
            {
                e = e->Oprev();
                DeleteEdge(e->Onext());
            }
            // Connect the new point to the vertices of the containing
            // triangle (or quadrilateral, if the new point fell on an
            // existing edge.)
            Edge<T>* base = MakeEdge(this);
            base->EndPoints(e->Org(), new Point<T>(x));
            Splice(base, e);
            startingEdge = base;
            do
            {
                base = Connect(e, base->Sym(), this);
                e = base->Oprev();
            }         while (e->Lnext() != startingEdge);
            // Examine suspect edges to ensure that the Delaunay condition
            // is satisfied.
            do
            {
                Edge<T>* t = e->Oprev();
                if (RightOf(t->Dest2d(), e) &&
                    InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x))
                {
                    Swap(e);
                    e = e->Oprev();
                }
                else if (e->Onext() == startingEdge) // no more suspect edges
                    return;
                else // pop a suspect edge
                    e = e->Onext()->Lprev();
            }         while (true);
        }
        // ===================================================================================================================
    } // end namespace math
} // end namespace zfs