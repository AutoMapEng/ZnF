#pragma once

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QObject>
SWITCH_WARNINGS_ON

#include <zfs/filter/filter_param.h>

#include "QToolsApi.h"

namespace zfqtools {
    namespace filter {

        /**
         * @deprecated
         */
        class QTOOLS_API FilterTranslator : public QObject {
            Q_OBJECT

                QString AngleParamDisplayName = QObject::tr("Angle");
            QString WriteReportFileParamDisplayName =
                QObject::tr("Write sign report file");
            QString MinimumPercentParamDisplayName = QObject::tr("Minimum [%]");
            QString MaximumPercentParamDisplayName = QObject::tr("Maximum [%]");
            QString MinimumIncParamDisplayName = QObject::tr("Minimum [inc]");
            QString MaximumIncParamDisplayName = QObject::tr("Maximum [inc]");

            QString Use2MasksParamDisplayName = QObject::tr("Use two masks");
            QString PercentParamDisplayName = QObject::tr("Values in [%]");

            QString FromParamDisplayName = QObject::tr("From");
            QString ToParamDisplayName = QObject::tr("To");
            QString UseDefaultParamDisplayName = QObject::tr("Use default");
            QString DetectJumpsParamDisplayName = QObject::tr("Detect jumps");
            QString ValueParamDisplayName = QObject::tr("Value");
            QString FilterStrengthParamDisplayName = QObject::tr("Filter strength");
            QString UseNoiseReductionParamDisplayName = QObject::tr("Quality booster");
            QString PixelParamDisplayName = QObject::tr("Pixel");
            QString DistanceParamDisplayName = QObject::tr("Distance");

            QString NormIntensityFilterDisplayName =
                QObject::tr("Normalized intensity filter");
            QString OverloadFilterDisplayName = QObject::tr("Overload filter");
            QString NoniusjumpFilterDisplayName = QObject::tr("Nonius filter");
            QString SkyFilterDisplayName = QObject::tr("Sky filter");
            QString MirrorFilterDisplayName = QObject::tr("Mirror filter");
            QString InfraredFilterDisplayName = QObject::tr("Infrared filter");

            QString HorizontalInvalidFilterDisplayName =
                QObject::tr("Horizontal invalid filter");
            QString VerticalInvalidFilterDisplayName =
                QObject::tr("Vertical invalid filter");

            QString IntensityFilterDisplayName = QObject::tr("Intensity filter");
            QString RangeFilterDisplayName = QObject::tr("Range filter");
            QString MixedPixelFilterDisplayName = QObject::tr("Mixed pixel filter");
            QString MixedPixelFilterEnhancedDisplayName =
                QObject::tr("Mixed pixel filter (enhanced)");
            QString SinglePixelFilterDisplayName = QObject::tr("Single pixel filter");
            QString ThinFilterDisplayName = QObject::tr("Thin filter");
            QString CriticalCompensatorDisplayName = QObject::tr("Critical compensation");

            QString ThreeDSelDisplayName = QObject::tr("3D volume selection");
            QString SelectionDisplayName = QObject::tr("Selection");

            QString OverloadDisplayName = QObject::tr("Overload");

        public:
            FilterTranslator(QObject *parent = NULL) : QObject(parent) {}

            QString getFilterName(const std::string& filterName);
            QString getFilterName(const std::wstring& filterName);
            QString getFilterName(const QString& filterName);
            QString getParameterName(const std::string& paramName);
        };

        //#########################################################################

        /**
         * @brief Helper class to get the translation texts into the translation
         *        files. The registered values have to be aligned with the keys
         *        of the filter parameters.
         */
#if 0 // obsolete
        class FilterTranslatorHelper : public QObject {
            Q_OBJECT

        public:
            FilterTranslatorHelper(QObject *parent = NULL) : QObject(parent) {

                QObject::tr("intensity");
                QObject::tr("minRf");
                QObject::tr("maxRf");

                QObject::tr("noniusCorr");
                QObject::tr("noniusIterations");

                QObject::tr("useNoniusFilter");
                QObject::tr("nNoniusS");
                QObject::tr("noniusAddDist");
                QObject::tr("noniusRfBand");

                QObject::tr("mixedpixel");
                QObject::tr("mix_d");
                QObject::tr("nptS");
                QObject::tr("nptNP");
                QObject::tr("bDiag");

                QObject::tr("range");
                QObject::tr("minRg");
                QObject::tr("maxRg");

                QObject::tr("jump");
                QObject::tr("jumpF");
                QObject::tr("nJumpsS");
                QObject::tr("nJumpsPN");

                QObject::tr("useSingleFilter");
                QObject::tr("nsinglePN");
                QObject::tr("nsingleS");
                QObject::tr("singleD");

                QObject::tr("useSingelMasked");
                QObject::tr("nSingelMaskedPN");
                QObject::tr("nSingelMaskedS");

                QObject::tr("useSmallObjectsFilter");
                QObject::tr("smallObjectsLines");
                QObject::tr("smallObjectsExtend");
                QObject::tr("smallObjectsNSigma");

                QObject::tr("overload");
                QObject::tr("overRf");
                QObject::tr("overPreInc");
                QObject::tr("overPostInc");

                QObject::tr("overloadRf");
                QObject::tr("overloadRfPixMargin");

                QObject::tr("rfrg");

                QObject::tr("edge");

                QObject::tr("badLinesFilter");

                //#########################################

                QObject::tr("intensity_helptext");
                QObject::tr("minRf_helptext");
                QObject::tr("maxRf_helptext");

                QObject::tr("noniusCorr_helptext");
                QObject::tr("noniusIterations_helptext");

                QObject::tr("useNoniusFilter_helptext");
                QObject::tr("nNoniusS_helptext");
                QObject::tr("noniusAddDist_helptext");
                QObject::tr("noniusRfBand_helptext");

                QObject::tr("mixedpixel_helptext");
                QObject::tr("mix_d_helptext");
                QObject::tr("nptS_helptext");
                QObject::tr("nptNP_helptext");
                QObject::tr("bDiag_helptext");

                QObject::tr("range_helptext");
                QObject::tr("minRg_helptext");
                QObject::tr("maxRg_helptext");

                QObject::tr("jump_helptext");
                QObject::tr("jumpF_helptext");
                QObject::tr("nJumpsS_helptext");
                QObject::tr("nJumpsPN_helptext");

                QObject::tr("useSingleFilter_helptext");
                QObject::tr("nsinglePN_helptext");
                QObject::tr("nsingleS_helptext");
                QObject::tr("singleD_helptext");

                QObject::tr("useSingelMasked_helptext");
                QObject::tr("nSingelMaskedPN_helptext");
                QObject::tr("nSingelMaskedS_helptext");

                QObject::tr("useSmallObjectsFilter_helptext");
                QObject::tr("smallObjectsLines_helptext");
                QObject::tr("smallObjectsExtend_helptext");
                QObject::tr("smallObjectsNSigma_helptext");

                QObject::tr("overload_helptext");
                QObject::tr("overRf_helptext");
                QObject::tr("overPreInc_helptext");
                QObject::tr("overPostInc_helptext");

                QObject::tr("overloadRf_helptext");
                QObject::tr("overloadRfPixMargin_helptext");

                QObject::tr("rfrg_helptext");

                QObject::tr("edge_helptext");

                QObject::tr("badLinesFilter_helptext");
            }

            /**
             * @brief Get translation of the given key
             * @param key the lookup key
             *
             * @return the translation or the key if no translation is available
             */
            QString getTranslationName(const QString &key);

            /**
             * @brief Get translation of the given key + extension
             *
             * @param key the lookup key
             *
             * @return the translation of 'KEY_helptext' or key if no translation is available
             */
            QString getTranslationText(const QString &key);
        };
#endif


        //#########################################################################

        /**
         * @brief Implementation of the filter translator which looks up translations
         *        via the keys. The keys have to be present in a MOCKED class to
         *        be present in the translation files.
         */
#if 0 // obsolete
        class QTOOLS_API FilterTranslatorImpl : public zfs::FilterTranslator {

        private:
            FilterTranslatorHelper helper;

        public:
            FilterTranslatorImpl() {}

            /**
             * @brief Get the translated name. If the translation is found by key,
             *        this value is returned. If no translation is available
             *        the BaseFilterTranslator is called.
             *
             * @param item  the item to be looked up
             *
             * @return the translation or the value found in the BaseFilterTranslator
             */
            virtual std::wstring getName(const zfs::FilterParamItem &item);

            /**
             * @brief Get the translated help text. If the translation is found
             *        by 'KEY_helptext', this value is returned. If no
             *        translation is available the BaseFilterTranslator is called.
             *
             * @param item  the item to be looked up
             *
             * @return the translation or the value found in the BaseFilterTranslator
             */
            virtual std::wstring getText(const zfs::FilterParamItem &item);
        };
#endif

    } // namespace filter
} // namespace zfqtools
