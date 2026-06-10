#pragma once

#include <string>
#include <iostream>

#include <Common/inttypes_.h>

#include <zfs/zfsapi.h>
#include <zfs/Math/auxilliary/exception.h>

typedef char          signed_byte;
typedef unsigned char byte;

#undef min
#undef max

namespace zfs {
    namespace math
    {
        //========================================================================================================================================================
        // Exception class for tuple (maybe specialization necessary)
        struct TupleException : public ZFSMathException
        {
            TupleException(const std::string& a_ExceptionString) : ZFSMathException(a_ExceptionString) {}
            ~TupleException() NOEXCEPT override {}
        };
        //========================================================================================================================================================
        /** \class Tuple2BaseMin
        * \brief  Class providing minimum basic functionality for Tuples 2D (Co-ordinates, Data, ...).
        *
        * \author Juergen Holzner
        *
        * Class providing minimum basic functionality for Tuples 2D (Co-ordinates, Data, ...). Only read access
        *
        */
        //========================================================================================================================================================
        template <class T> class ZFS_API Tuple2MinBase
        {
        protected:
            T first;
            T second;

        public:
            // constructors
            Tuple2MinBase();
            Tuple2MinBase(const T& value);
            Tuple2MinBase(const T& a_first, const T& a_second);
            virtual ~Tuple2MinBase();

            // getter functions
            T getElement(const int idx) const;

            const T First()  const;
            const T Second() const;
            const T Last()   const;

            const T operator[](const int idx) const;
            const T operator()(const int idx) const;

            // operators
            bool operator==(const Tuple2MinBase<T>& rhs) const;
            bool operator!=(const Tuple2MinBase<T>& rhs) const;
        };

        //========================================================================================================================================================
        /** \class Tuple2Base
        * \brief  Class providing basic functionality for Tuples 2D (Co-ordinates, Data, ...).
        *
        * \author Juergen Holzner
        *
        * Class providing basic functionality for Tuples 2D (Co-ordinates, Data, ...).
        *
        */
        //========================================================================================================================================================
        template <class T> class ZFS_API Tuple2Base : public Tuple2MinBase<T>
        {

        public:
            // constructors
            Tuple2Base();
            Tuple2Base(const T& value);
            Tuple2Base(const T& a_first, const T& a_second);
            ~Tuple2Base() override;

            const T operator[](const int idx) const;
            const T operator()(const int idx) const;

            T& operator[](const int idx);
            T& operator()(const int idx);

            // setter functions
            void setElement(const int idx, const T& value);
            void setFirst(const T& value);
            void setSecond(const T& value);
            void setLast(const T& value);
        };

        //========================================================================================================================================================
        /** \class Tuple2
         * \brief  Class providing functionality for Tuples 2D (Co-ordinates, Data, ...).
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for Tuples 2D (Co-ordinates, Data, ...).
         *
         */
        template <class T> class ZFS_API Tuple2 : public Tuple2Base<T>
        {
        public:
            // constructors
            Tuple2();
            Tuple2(const T& value);
            Tuple2(const T& a_first, const T& a_second);
            Tuple2(const Tuple2<T>& src);
            ~Tuple2() override;

            template <class T1> Tuple2& operator/=(const T1& aDiv);
            template <class T1> Tuple2& operator*=(const T1& aMult);
            template <class T1> Tuple2& operator-=(const T1& aSub);
            template <class T1> Tuple2& operator+=(const T1& aAdd);

            const Tuple2<int> round() const;

            T max() const;
            T min() const;

        };

        //========================================================================================================================================================
        template <class T, class T1> ZFS_API const Tuple2<T>    operator/(const Tuple2<T>& lhs, const T1& rhs);
        template <class T, class T1> ZFS_API const Tuple2<T>    operator*(const Tuple2<T>& lhs, const T1& rhs);
        template <class T, class T1> ZFS_API const Tuple2<T>    operator-(const Tuple2<T>& lhs, const T1& rhs);
        template <class T, class T1> ZFS_API const Tuple2<T>    operator+(const Tuple2<T>& lhs, const T1& rhs);

        template <class T, class T1> ZFS_API const Tuple2<T>    operator%(const Tuple2<T>& lhs, const T1& rhs); // only for int

        template <class T, class T1> ZFS_API const Tuple2<bool> operator> (const Tuple2<T>& lhs, const Tuple2<T1>& rhs);
        template <class T, class T1> ZFS_API const Tuple2<bool> operator< (const Tuple2<T>& lhs, const Tuple2<T1>& rhs);
        template <class T, class T1> ZFS_API const Tuple2<bool> operator>=(const Tuple2<T>& lhs, const Tuple2<T1>& rhs);
        template <class T, class T1> ZFS_API const Tuple2<bool> operator<=(const Tuple2<T>& lhs, const Tuple2<T1>& rhs);

        template <class T, class T1> ZFS_API const Tuple2<bool> operator> (const Tuple2<T>& lhs, const T1& rhs);
        template <class T, class T1> ZFS_API const Tuple2<bool> operator< (const Tuple2<T>& lhs, const T1& rhs);
        template <class T, class T1> ZFS_API const Tuple2<bool> operator>=(const Tuple2<T>& lhs, const T1& rhs);
        template <class T, class T1> ZFS_API const Tuple2<bool> operator<=(const Tuple2<T>& lhs, const T1& rhs);
        //========================================================================================================================================================
        /** \class Tuple2
        * \brief  Class providing functionality for Tuples 2D (Co-ordinates, Data, ...).
        *
        * \author Juergen Holzner
        *
        * Class providing functionality for Tuples 2D (Co-ordinates, Data, ...).
        *
        */
        template<> class ZFS_API Tuple2<bool> : public Tuple2Base<bool>
        {
        public:
            // constructors
            Tuple2();
            Tuple2(const bool& value);
            Tuple2(const bool& a_first, const bool& a_second);
            ~Tuple2() override;

            bool all(); // all true
            bool any(); // any true
        };
        //========================================================================================================================================================
        /** \class TwoTypeTuple2
         * \brief  Class providing functionality for a Tuple of two elements having each a different type.
         *
         * \author Juergen Holzner
         *
         */
        template <class T1, class T2> class TwoTypeTuple2
        {
        protected:
            T1 first;
            T2 second;
        public:
            // constructors
            TwoTypeTuple2() :first(T1(0)), second(T2(0)) {}
            TwoTypeTuple2(const T1& value) :first(T1(value)), second(T2(0)) {}
            TwoTypeTuple2(const T2& value) :first(T1(0)), second(T2(value)) {}
            TwoTypeTuple2(const T1& a_first, const T2& a_second) :first(T1(a_first)), second(T2(a_second)) {}
            virtual ~TwoTypeTuple2() {}

            // getter functions
            const T1 First()  const { return first; }
            const T1 Second() const { return second; }

            T1& First() { return first; }
            T2& Second() { return second; }

            // setter functions
            void setFirst(const T1& value) { first = value; }
            void setSecond(const T2& value) { second = value; }

            // operators
            bool operator==(const TwoTypeTuple2<T1, T2>& rhs) const { return  (rhs.first == first && rhs.second == second); }
            bool operator!=(const TwoTypeTuple2<T1, T2>& rhs) const { return !(*this == rhs); }
        };

        //========================================================================================================================================================
        /** \class Tuple3MinBase
         * \brief  Class providing minimum basic functionality for Tuples 3D (Co-ordinates, Data, ...).
         *
         * \author Juergen Holzner
         *
         * Class providing minimum basic functionality for Tuples 3D (Co-ordinates, Data, ...). For const Types.
         *
         */
        template<class T> class ZFS_API Tuple3MinBase : public Tuple2MinBase<T>
        {
        protected:
            T third;
        public:
            // constructors and destructor
            Tuple3MinBase();
            Tuple3MinBase(const T& value);
            Tuple3MinBase(const T& a_first, const T& a_second, const T& a_third);
            ~Tuple3MinBase() override;

            // getter functions
            const T getElement(const int idx) const;

            const T Third()  const;
            const T Last()   const;

            // operators
            const T  operator[](const int idx) const;
            const T  operator()(const int idx) const;

            bool operator==(const Tuple3MinBase<T>& rhs) const;
            bool operator!=(const Tuple3MinBase<T>& rhs) const;

        };
        //========================================================================================================================================================
        /** \class Tuple3
         * \brief  Class providing functionality for Tuples 3D (Co-ordinates, Data, ...).
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for Tuples 3D (Co-ordinates, Data, ...).
         *
         */
        template<class T> class ZFS_API Tuple3 : public Tuple2Base<T> {
        protected:
            T third;
        public:
            // constructors and destructor
            Tuple3();
            Tuple3(const T& value);
            Tuple3(const T& a_first, const T& a_second, const T& a_third);
            virtual ~Tuple3() override;

            // getter functions
            const T getElement(const int idx) const;

            const T Third()  const;
            const T Last()   const;

            // setter functions
            void set(const T& first, const T& second, const T& third);
            void setElement(const int idx, const T& value);

            void  setThird(const T& value);
            void   setLast(const T& value);

            // operators
            const T  operator[](const int idx) const;
            T& operator[](const int idx);
            const T  operator()(const int idx) const;
            T& operator()(const int idx);

            bool operator==(const Tuple3<T>& rhs) const;
            bool operator!=(const Tuple3<T>& rhs) const;

        };
        //========================================================================================================================================================
        // operators
        //========================================================================================================================================================
        template<typename T> ZFS_API std::istream& operator >> (std::istream& is, Tuple2<T>& v);
        template<typename T> ZFS_API std::istream& operator >> (std::istream& is, Tuple3<T>& v);
        template<typename T> ZFS_API std::ostream& operator << (std::ostream& ofs, const Tuple3<T>& v);
        template<typename T> ZFS_API std::ostream& operator << (std::ostream& ofs, const Tuple2<T>& v);
        //========================================================================================================================================================
    } // end namespace math
} // end namespace zfs
