#pragma once

#include <zfs/zfsapi.h>

#include <vector>

#include <zfs/Math/math/polynomial.h>

namespace zfs { 
    namespace math {
        // ====================================================================================
        enum PolyAdressSet
        {
            PPF_Start = 0, PPF_Overlap1Start, PPF_Overlap1End, PPF_Overlap2Start, PPF_Overlap2End, PPF_End
        };
        // ====================================================================================
        /*
        * @brief Class for fitting polynomials to data using subsections
        * @author J. Holzner
        */
        template<class T> class ZFS_API PartialPolyFit
        {
        public:
            PartialPolyFit();
            ~PartialPolyFit();

            PartialPolyFit(const int nSamples,
                const int nSections,
                const int nOverlapValid,
                const int nEdge,
                const int nSubPolyOrder);

            void setData(const std::vector<T>& samplePosFit, const std::vector<T>& samplePosEval, const std::vector<T>& sampleVals); /// < when the polynomials should be evaluated at the same positions, then set samplePosEval= samplePosFit;
            void setPolyOrder(const int nSubPolyOrder);
            void changeSectionSettings(const int nSamples, const int nSections, const int nOverlapValid, const int nEdge);

            // parameter settings
            int                                 getNoOfSamples()       const;
            int                                 getSubPolyOrder()      const;
            int                                 getNoOfSections()      const;
            int                                 getOverlapValid()      const;
            int                                 getEdge()              const;
            int                                 getOverlapTotal()      const;

            const std::vector<std::vector<int>> getPolySectionRanges() const;

            // results
            T getValue(const int samplePos) const; /// < refers to samplePosEval
            T operator[](const int samplePos) const; /// < refers to samplePosEval

            const std::vector<Polynomial> getPolies() const;
            const std::vector<T>          getStdDevs() const;

            const std::vector<T>          getFittedValues() const;

        private:

            // input
            int m_nSamples;
            int m_nSections;
            int m_nOverlapValid;
            int m_nEdge;
            int m_nSubPolyOrder;

            // derived
            int m_nOverlapTotal;
            int m_nPartial;

            // calculated
            std::vector<std::vector<int>> m_PolySectionRanges;
            std::vector<Polynomial>       m_Polynomials;
            std::vector<T>                m_StdDevs;
            std::vector<T>                m_CenterPosPoly;
            std::vector<T>                m_CenterValPoly;

            std::vector<T>                m_PolyValues;

            // preparations
            void basicInitialisation();
            void calcSectionSize();
            void calcPolySectionRanges();
            void conductCalculations(const std::vector<T>& samplePosFit, const std::vector<T>& samplePosEval, const std::vector<T>& sampleVals);

            void fitPolynomials(const std::vector<T>& samplePosFit, const std::vector<T>& sampleVals, const int nSubPolyOrder);
            void fitPolynomial(const int PolyIdxToFit, const std::vector<T>& samplePosFit, const std::vector<T>& sampleVals, const int nSubPolyOrder);

            void fitPolynomialsMT(const std::vector<T>& samplePosFit, const std::vector<T>& sampleVals, const int nSubPolyOrder);
            static void fitPolyThreadWorker(PartialPolyFit<T>* pPartialPolyFit, const int PolyFitIdx, const std::vector<T>* const samplePosFit, const std::vector<T>* const sampleVals, const int* const nSubPolyOrder);


            // for evaluations
            void evaluateAndCombinePartials(const std::vector<T>& samplePosEval);
        };
        // ====================================================================================
    } // end namespace math
} // end namespace zfs