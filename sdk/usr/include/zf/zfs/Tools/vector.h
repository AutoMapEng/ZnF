#ifndef VECTOR_H
#define VECTOR_H

namespace zfuk
{
#include "table.h"


    template<class T> class vector;
    //template<class T> istream &operator >> (istream &, vector<T> &);
    template<class T> class subvector;
    //template<class T> istream &operator >> (istream &, subvector<T> &);



    template<class T> class subvector : public subtable<T> {
        void use();
    public:
        friend class vector<T>;

        subvector(int, T *);
        subvector(const subvector<T> &, int = 0, int = -1);
        subvector(const vector<T> &);

        subvector<T> &operator = (const subvector<T> &);
        subvector<T> &operator = (const vector<T> &);
        subvector<T> &operator = (const T &);

        subvector<T> &operator += (const subvector<T> &);
        subvector<T> &operator += (const vector<T> &);

        subvector<T> &operator -= (const subvector<T> &);
        subvector<T> &operator -= (const vector<T> &);

        subvector<T> &operator *= (const T &);
        subvector<T> &operator /= (const T &);

        vector<T> operator + (const subvector<T> &) const;
        vector<T> operator + (const vector<T> &) const;

        vector<T> operator - (const subvector<T> &) const;
        vector<T> operator - (const vector<T> &) const;

        vector<T> operator - () const;
        vector<T> operator * (const T &) const;
        vector<T> operator / (const T &) const;

        T operator * (const subvector<T> &) const;
        T operator * (const vector<T> &) const;

        //  friend istream &operator >> (istream &, subvector<T> &);
    };

    template<class T> inline subvector<T>::subvector(int size, T *a) :
        subtable<T>(size, a) { }

    template<class T> inline subvector<T>::subvector
    (const subvector<T> &s, int from, int to) :
        subtable<T>(s, from, to) { }

    template<class T> inline subvector<T> &subvector<T>::operator =
        (const subvector<T> &s) {
        subtable<T>::operator = (s);
        return *this;
    }

    template<class T> inline subvector<T> &subvector<T>::operator =
        (const vector<T> &v) {
        subtable<T>::operator = (v);
        return *this;
    }

    template<class T> inline subvector<T> &subvector<T>::operator = (const T &t) {
        subtable<T>::operator = (t);
        return *this;
    }

    template<class T> inline subvector<T> &subvector<T>::operator +=
        (const subvector<T> &s) {
        int i;
        for (i = 0; i < this->sz; i++) {
            this->array[i] += s.array[i];
        }
        return *this;
    }

    template<class T> inline subvector<T> &subvector<T>::operator +=
        (const vector<T> &v) {
        return *this += subvector<T>(v);
    }

    template<class T> inline subvector<T> &subvector<T>::operator -=
        (const subvector<T> &s) {
        int i;
        for (i = 0; i < this->sz; i++) {
            this->array[i] -= s.array[i];
        }
        return *this;
    }

    template<class T> inline subvector<T> &subvector<T>::operator -=
        (const vector<T> &v) {
        return *this -= subvector<T>(v);
    }


    template<class T> inline subvector<T> &subvector<T>::operator *=
        (const T &s) {
        int i;
        for (i = 0; i < this->sz; i++) {
            //array[i] *= s ersetzt durch: (AA30.5.00)
            this->array[i] = this->array[i] * s;
        }
        return *this;
    }
    /*
    template<class T> inline subvector<T> &subvector<T>::operator /=
    (const T &s) {
      int i;
      for(i=0; i<sz; i++) {
    //array[i] /= s ersetzt durch: (AA30.5.00)(funktioniert auch nicht)
          array[i] = array[i]/s;
      }
      return *this;
    }
    */
    template<class T> inline T subvector<T>::operator * (const subvector<T> &s) const {
        int i;
        T sum = 0;
        for (i = 0; i < this->sz; i++) {
            sum += this->array[i] * s.array[i];
        }
        return sum;
    }

    template<class T> inline T subvector<T>::operator * (const vector<T> &v) const {
        //  return *this * subvector<T>(v);
        int i;
        T sum = 0;
        for (i = 0; i < this->sz; i++) {
            sum += this->array[i] * v.array[i];
        }
        return sum;
    }


    //================================================================


    template<class T> class vector : public table<T> {
        void use();
    public:
        friend class subvector<T>;

        vector(int = 1);
        vector(const subvector<T> &);
        vector(const vector<T> &);

        vector<T> &operator = (const subvector<T> &s) {
            table<T>::operator = (s);
            return *this;
        }
        vector<T> &operator = (const vector<T> &v) {
            table<T>::operator = (v);
            return *this;
        }

        vector<T> &operator = (const T & t) {
            table<T>::operator = (t);
            return *this;
        }

        vector<T> &operator += (const subvector<T> &s) {
            subvector<T>(*this) += s;
            return *this;
        }
        vector<T> &operator += (const vector<T> &v) {
            subvector<T>(*this) += v;
            return *this;
        }

        vector<T> &operator -= (const subvector<T> &s) {
            subvector<T>(*this) -= s;
            return *this;
        }
        vector<T> &operator -= (const vector<T> &v) {
            subvector<T>(*this) -= v;
            return *this;
        }

        vector<T> &operator *= (const T &t) {
            subvector<T>(*this) *= t;
            return *this;
        }
        vector<T> &operator /= (const T &t) {
            subvector<T>(*this) /= t;
            return *this;
        }

        vector<T> operator + (const subvector<T> &s) const {
            vector<T> temp(*this);
            return temp += s;
        }
        vector<T> operator + (const vector<T> &v) const {
            vector<T> temp(*this);
            return temp += v;
        }

        vector<T> operator - (const subvector<T> &s) const {
            vector<T> temp(*this);
            return temp -= s;
        }
        vector<T> operator - (const vector<T> &v) const {
            vector<T> temp(*this);
            return temp -= v;
        }
        vector<T> operator - () const {
            vector<T> temp(*this);
            int i;
            for (i = 0; i < this->sz; i++) temp.array[i] *= -1;
            return temp;
        }

        vector<T> operator * (const T &t) const {
            vector<T> temp(*this);
            return temp *= t;
        }
        vector<T> operator / (const T &t) const {
            vector<T> temp(*this);
            return temp /= t;
        }

        T operator * (const subvector<T> &) const;
        T operator * (const vector<T> &) const;

        //  friend istream &operator >> (istream &, vector<T> &);
    };

    template<class T> inline vector<T>::vector(int size) :
        table<T>(size) { }

    template<class T> inline vector<T>::vector(const subvector<T> &s) :
        table<T>(s) { }

    template<class T> inline vector<T>::vector(const vector<T> &v) :
        table<T>(v) { }

    template<class T> inline T vector<T>::operator * (const subvector<T> &s)
        const {
        int i;
        T sum = 0;
        for (i = 0; i < this->sz; i++) {
            sum += this->array[i] * s.array[i];
        }
        return sum;
    }

    template<class T> inline T vector<T>::operator * (const vector<T> &v) const {
        int i;
        T sum = 0;
        for (i = 0; i < this->sz; i++) {
            sum += this->array[i] * v.array[i];
        }
        return sum;
    }

    template<class T> inline subvector<T>::subvector(const vector<T> &v) :
        subtable<T>(v.sz, v.array) { }

    template<class T> inline vector<T> subvector<T>::operator +
        (const subvector<T> &s) const {
        vector<T> temp(*this);
        return temp += s;
    }

    template<class T> inline vector<T> subvector<T>::operator -
        (const subvector<T> &s) const {
        vector<T> temp(*this);
        return temp -= s;
    }

    template<class T> inline vector<T> subvector<T>::operator - () const {
        vector<T> temp(*this);
        int i;
        for (i = 0; i < this->sz; i++) temp.array[i] *= -1;
        return temp;
    }

    template<class T> inline vector<T> subvector<T>::operator * (const T &c)
        const {
        vector<T> temp(*this);
        return temp *= c;
    }

    template<class T> inline vector<T> subvector<T>::operator / (const T &c)
        const {
        vector<T> temp(*this);
        return temp /= c;
    }

#ifdef VECTOR_IMPL
    /*
    template<class T> istream &operator >> (istream &is, subvector<T> &s) {
      T zwischen;
      int i;
      for(i=0; i<s.sz && (is >> zwischen); i++) s.array[i] = zwischen;
      return is;
    }

    template<class T> istream &operator >> (istream &is, vector<T> &v) {
      T zwischen;
      int i;
      for(i=0; i<v.sz && (is >> zwischen); i++) v.array[i] = zwischen;
      return is;
    }
    */
    template<class T> void subvector<T>::use() {
        //  cin >> *this;
        //  cout << *this;
    }

    template<class T> void vector<T>::use() {
        //  cin >> *this;
        //  cout << *this;
    }


#endif
} // namespace

#endif
