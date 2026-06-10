#ifndef CADVECTOR_H
#define CADVECTOR_H

#include "inlines.h"
#include "vector.h"
#include <math.h>
/*
#ifndef _ads_h
#include "adslib.h"
#endif
*/

/*
n-dim. (double-)Vektoren mit Norm...
3-dim. Vektoren mit Polarkoord.-Fkt., Kreuzprodukt...
*/

namespace zfuk
{
    class lVektor : public vector<long int> {
    public:

        lVektor(const int d) { set_dimension(d); }

        double norm() const { return norm(*this); }

        //heikel bei double-Werten!:
        BOOL operator==(const lVektor &VOp) const {
            if ((this->dimension() == VOp.dimension()) && (norm(*this - VOp) == 0.0)) return TRUE; else return FALSE;
        }
        //evtl. ==-Operator definieren, der Gleichheit innerhalb einer einstellbaren Schranke prüft!

    private:
        double norm(const vector<long int>& V) const { return sqrt((double)(V*V)); }//aus irgendwelchen Gründen darf V hier kein dVektor sein
        //siehe CADVektor::unity() - hat was mit Typen und casts zu tun (und mit Copy-Konstr.?)
    };


    class dVektor : public vector<double> {
    public:
        //Konstruktoren:
        /*
            dVektor(const int);
            dVektor(const dVektor &V){}
        */
        friend class CADVektor;
        //Funktionen
        double norm() const { return norm(*this); }
        void nullify() { for (int i = 0; i < sz; i++) array[i] = 0; }
        //Operatoren:

            //heikel bei double-Werten!:
        BOOL operator==(const dVektor &VOp) const {
            if ((this->dimension() == VOp.dimension()) && (norm(*this - VOp) == 0.0)) return TRUE; else return FALSE;
        }
        //evtl. ==-Operator definieren, der Gleichheit innerhalb einer einstellbaren Schranke prüft!
        //oder Funktion, z.B. .equals(dVektor Zwo, int SigDigs)

    private:
        double norm(const vector<double>& V) const { return sqrt(V*V); }//aus irgendwelchen Gründen darf V hier kein dVektor sein
        //siehe CADVektor::unity() - hat was mit Typen und casts zu tun (und mit Copy-Konstr.?)
    };


    class CADVektor : public dVektor {
    public:
        //Konstruktoren:
        inline CADVektor() { set_dimension(3); }
        CADVektor(double x, double y, double z) { set_dimension(3); SetCart(x, y, z); }
        //Copy-Konstruktoren:
        CADVektor(const CADVektor &V) { set_dimension(3); for (int i = 0; i < 3; i++) array[i] = V[i]; }
        CADVektor(const vector<double> &V) { if (V.dimension() == 3) { set_dimension(3); for (int i = 0; i < 3; i++) array[i] = V[i]; } }


        void inline Set(const CADVektor &V) { for (int i = 0; i < 3; i++) array[i] = V[i]; }
        void inline SetCart(const double& fx, const double& fy, const double& fz) { array[0] = fx; array[1] = fy; array[2] = fz; }
        void inline SetCart(const float& fx, const float& fy, const float& fz) { array[0] = (double)fx; array[1] = (double)fy; array[2] = (double)fz; }

        //Abstand
        double abstand(const CADVektor &V) const { CADVektor temp = V - *this; return temp.norm(); }
        //falls jemand sichergehen will:
        double inline x() const { return array[0]; }
        double inline y() const { return array[1]; }
        double inline z() const { return array[2]; }
        //acad	ads_point inline *adspointer() const {return (ads_point *)array;}

        //Theta,Phi in rad:
        void SetPolar(const double&, const double&, const double&);
        double theta() const;
        double phi() const;
        double inline r() const { return this->norm(); }

        //unity:zugehöriger Einheitsvektor; unity,ortho1,ortho2: paarweise orthonormal (und leider linkshändig!)
        CADVektor unity() const;
        CADVektor ortho1() const;
        CADVektor ortho2() const;

        //Kreuzprodukt:
        CADVektor operator^(CADVektor &);

        //Nullvektor,Einheitsvektoren:
        static CADVektor const Null;
        static CADVektor const ex;
        static CADVektor const ey;
        static CADVektor const ez;

    private:
        //spez. Signum-Fkt. für Theta,Phi-Intervalle:
        double sign(const double& d) const { if (d > 0.0)return 1.0; else return -1.0; }

        //Index-Shifts für Kreuzprodukt:
        int cyc(int iKomp) { iKomp += 1; if (iKomp < sz) return iKomp; else return 0; }
        int recyc(int iKomp) { iKomp -= 1; if (iKomp >= 0) return iKomp; else return (sz - 1); }
    };
} // namespace

#endif
