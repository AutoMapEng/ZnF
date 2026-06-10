#ifndef TABLE_H
#define TABLE_H

//#include <stdlib.h>
//#include <iostream.h>

//#pragma interface

#define CHECKALLOC

//template<class T> class vector_base;
//template<class T> ostream &operator << (ostream &, const vector_base<T> &);

template<class T> class vector_base
{
protected:
    int sz;
    T *array;
    vector_base() : sz(0), array(0) { }

    virtual void abstractor() = 0;
    void use();

public:
    vector_base(const vector_base<T> &v) : sz(v.sz), array(v.array) { }
    vector_base(int size, T *a);
    virtual ~vector_base() {}

    T &operator [] (int) const;
    vector_base<T> &operator = (const vector_base<T> &);
    vector_base<T> &operator = (const T &);
    void bind(const vector_base<T> &, int = 0, int = -1);
    virtual int dimension() const;
    virtual int operator ! () const;
    virtual void set_dimension(int);

    //  friend ostream &operator << (ostream &, const vector_base<T> &);
    //  friend istream &operator >> (istream &, vector_base<T> &);
};

template<class T> inline vector_base<T>::vector_base(int size, T *a) :
    sz(size), array(a) {
#ifdef CHECKALLOC
    if (!a) {
        //    cerr << "Nicht genuegend Speicherplatz fuer einen Vektor ";
        //    cerr << "oder Initialisierung mit Adresse 0!" << endl;
        //    cerr << "Programm vorzeitig beendet." << endl;
        //    exit(1);
    }
#endif
}

template<class T> inline T &vector_base<T>::operator [] (int index) const {
    return array[index];
}

template<class T> inline vector_base<T> &vector_base<T>::operator =
(const vector_base<T> &v) {
    int i;
    for (i = 0; i < sz; i++) {
        array[i] = v.array[i];
    }
    return *this;
}

template<class T> inline vector_base<T> &vector_base<T>::operator =
(const T &t) {
    int i;
    for (i = 0; i < sz; i++) {
        array[i] = t;
    }
    return *this;
}

template<class T> inline void vector_base<T>::bind(const vector_base<T> &v,
    int from, int to) {
    sz = (to > 0 ? to : v.sz) - from;
    array = v.array + from;
}

template<class T> inline int vector_base<T>::dimension() const {
    return sz;
}

template<class T> inline int vector_base<T>::operator ! () const {
    return !array;
}

template<class T> inline void vector_base<T>::set_dimension(int d) {
    sz = d;
}

#ifdef TABLE_IMPL

// die beiden stream-Operatoren >> und << muessen benutzt werden, da der
// Compiler sie sonst nicht erzeugt, daher folgende Funktion:
template<class T> void vector_base<T>::use() {
    //  cin >> *this;
    //  cout << *this << endl;
}
/*
template<class T> ostream &operator << (ostream &os, const vector_base<T> &v) {
  int i;
  for(i=0; i<v.sz-1; i++) os << v.array[i] << " ";
  return os << v.array[v.sz-1];
}
*/
/*
template<class T> istream &operator >> (istream &is, vector_base<T> &v) {
  int i;
  T zwischen;
  for(i=0; i<v.sz && (cin >> zwischen); i++) v.array[i] = zwischen;
  return is;
}
*/

#endif

template<class T> class table;

template<class T> class subtable : public vector_base<T> {
    void abstractor() { }

public:
    friend class table<T>;

    subtable(int size, T *a) : vector_base<T>(size, a) { }
    subtable(const subtable<T> &, int = 0, int = -1);
    subtable(const table<T> &);

    subtable<T> &operator = (const subtable<T> &);
    subtable<T> &operator = (const table<T> &);
    subtable<T> &operator = (const T &);
};

template<class T> inline subtable<T>::subtable
(const subtable<T> &s, int from, int to) :
    vector_base<T>((to > 0 ? to : s.sz) - from, s.array + from) { }

template<class T> inline subtable<T>
    &subtable<T>::operator = (const subtable<T> &s) {
        vector_base<T>::operator = (s);
        return *this;
    }

    template<class T> inline subtable<T> &subtable<T>::operator =
        (const table<T> &t) {
        vector_base<T>::operator = (t);
        return *this;
    }

    template<class T> inline subtable<T> &subtable<T>::operator = (const T &t) {
        vector_base<T>::operator = (t);
        return *this;
    }





    template<class T> class table : public vector_base<T> {
        void abstractor() { }

    public:
        friend class subtable<T>;

        table(int size = 1) : vector_base<T>(size, new T[size]) { }
        table(const subtable<T> &);
        table(const table<T> &);
        virtual ~table() { delete[] this->array; }

        table<T> &operator = (const subtable<T> &);
        table<T> &operator = (const T &);

        void set_dimension(int);
    };


    template<class T> inline table<T>::table(const subtable<T> &s) :
        vector_base<T>(s.sz, new T[s.sz]) {
        vector_base<T>::operator = (s);
    }

    template<class T> inline table<T>::table(const table<T> &t) :
        vector_base<T>(t.sz, new T[t.sz]) {
        vector_base<T>::operator = (t);
    }

    template<class T> inline table<T> &table<T>::operator =
        (const subtable<T> &s) {
        if (this->sz != s.sz) {
            delete[] this->array;
            this->array = new T[this->sz = s.sz];
        }
        vector_base<T>::operator = (s);
        return *this;
    }

    template<class T> inline table<T> &table<T>::operator = (const T &t) {
        vector_base<T>::operator = (t);
        return *this;
    }

    template<class T> inline subtable<T>::subtable(const table<T> &t) :
        vector_base<T>(t.sz, t.array) { }

    template<class T> inline void table<T>::set_dimension(int d) {
        if (d != this->sz) {
            delete[] this->array;
            this->array = new T[this->sz = d];
        }
    }

#endif
