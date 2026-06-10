#ifndef __ZFS_MATRIX_H__
#define __ZFS_MATRIX_H__

//////////////////////////////////////////////////////////////////////
//
// Matrix.h: Schnittstelle für die Klasse Matrix.
//
//////////////////////////////////////////////////////////////////////

#include "../config_zfs.h"

#ifdef ZFS_MATRIX

#include "table.h"
#include "vector.h"
#include "cadvektor.h"

namespace zfuk
{
    //ostream &operator << (ostream &, const Matrix &);

    class Matrix
    {
    public:
        //Konstruktoren:
            //leer:
        Matrix(int, int);
        Matrix(int);//quadratisch
        Matrix() {}
        //gesetzt:
        Matrix(table<dVektor> &);
        Matrix(CADVektor &, CADVektor &, CADVektor &);
        //Copy-Konstruktor:
        Matrix(const Matrix &);

        //Destruktor:
        virtual ~Matrix();

        //Funktionen:

            //n-ter Spaltenvektor: (die Zeilenvektoren ergeben sich über den []-Operator)
        vector<double> Spalte(int) const;
        //n-ter Zeilenvektor: (eigtl. unnötig; trotzdem aus Symmetriegründen)
        vector<double> Zeile(int i) const { return M[i]; }

        //Größe der Matrix:
        void set_dimension(int i, int j);
        //Test mit Umdrehen
    //	int inline Spalten() const {return (M.dimension());}
    //	int inline Zeilen() const {return (M[0].dimension());}
        int inline Spalten() const { return (M[0].dimension()); }
        int inline Zeilen() const { return (M.dimension()); }
        lVektor dims() const { lVektor d(2); d[0] = Zeilen(); d[1] = Spalten(); return d; }
        bool inline IsSquare() const { return (Zeilen() == Spalten()); }

        //Zeilen vertauschen:
        void Zeilentausch(int, int);

        //Spur:
        double Spur() const;

        //transponierte Matrix:
        Matrix T() const;

        //Inverse Matrix:
        Matrix Inverse() const;

        // Diagonalelemente
        Matrix Diagonale() const;

        // Maximale Element
        double Mat_Max(int&, int&) const; // double max_wert = Max(spalte, zeile)

        // Minimale Element
        double Mat_Min(int&, int&) const; // double min_wert = Max(spalte, zeile)

        //weitere Funktionen: det, , Eigenval, Lösungsvektor (incl. inhom.),...


    //Operatoren:
        vector<double> &operator [] (int) const;

        vector<double> operator * (vector<double> &);
        dVektor operator * (dVektor &);
        CADVektor operator * (const CADVektor &) const;

        Matrix operator * (double);

        Matrix operator * (const Matrix &);
        Matrix operator + (Matrix &);
        Matrix operator - (Matrix &);

        Matrix &operator = (const Matrix &);

        //	friend ostream &operator << (ostream &, Matrix &);

        //n-dim. Einheitsmatrix als statische Elementfunktion:
        static Matrix Unity(int);
        static const Matrix F3;

    protected:
        vector<vector<double> > M;

    private:
        double inline abs(double d) const { if (d < 0.0) return (-d); else return d; }
    };
}

#endif // ZFS_MATRIX

#endif //__REGISTER_MATRIX_H__
