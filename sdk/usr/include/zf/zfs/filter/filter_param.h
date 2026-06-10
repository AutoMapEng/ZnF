#pragma once
#include "../zfsapi.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <list>

#include "filter_id.h"
#include "../flags.h"

class ZFSFilter;
class ZFSHeader;

namespace tinyxml2
{
    class XMLNode;
}

namespace zfs
{

class FilterParams;

/**
\brief for standardfilter already defined filter param keys
*/
    struct ZFS_API FilterParam {
        static const char *rf;
        static const char *minRf;
        static const char *maxRf;

        static const char* rfNorm;
        static const char* rfNormMin;
        static const char* rfNormMax;

        static const char *nonCorr;
        static const char *noniusIterations;

        static const char *useNoniusFilter;
        static const char *nNoniusS;
        static const char *noniusAddDist;
        static const char *noniusRfBand;
        static const char* noniusPreRemove;
        static const char* noniusPostRemove;

        static const char *mixedpixel;
        static const char *mix_d;
        static const char *nptS;
        static const char *nptNP;
        static const char *bDiag;

        static const char *spikes;
        static const char *spikesLimit1;
        static const char *spikesLimit2;

        static const char *range;
        static const char *minRg;
        static const char *maxRg;

        static const char* sigproc;
        static const char* sigproc_weak;
        static const char* sigproc_stability;
        static const char* sigproc_strong;
        static const char* sigproc_IP;

        static const char *jump;
        static const char *jumpF;
        static const char *nJumpsS;
        static const char *nJumpsPN;

        static const char *useSingleFilter;
        static const char* useSingleFilterAdvanced;
        static const char *nsinglePN;
        static const char *nsingleS;
        static const char *singleD;

        static const char *useSingelMasked;
        static const char *nSingelMaskedPN;
        static const char *nSingelMaskedS;

        static const char *useSmallObjectsFilter;
        static const char *smallObjectsLines;
        static const char *smallObjectsExtend;
        static const char *smallObjectsNSigma;

        static const char *overload;
        static const char *overRf;
        static const char *overPreInc;
        static const char *overPostInc;

        static const char *overloadRf;
        static const char *overloadRfPixMargin;

        static const char *rfrg;

        static const char *edge;

        static const char *badLinesFilter;

        static const char* vertValid;
        static const char* vertValidFrom1;
        static const char* vertValidTo1;
        static const char* vertValidFrom2;
        static const char* vertValidTo2;

        static const char* thin;
        static const char* thinGridSize;
        static const char* thinMinSub;

        static const char* compensator;

        static const char* mxpxline;
//        static const char* mxpxline_ver;
        static const char* mxpxline_wid;
        static const char* mxpxline_w5;
        static const char* mxpxline_w2;
        static const char* mxpxline_w0;
        static const char* mxpxline_g5;
        static const char* mxpxline_g2;
        static const char* mxpxline_g0;

        static const char* xyz_in;
        static const char* xyz_in_top;
        static const char* xyz_in_bottom;
        static const char* xyz_in_left;
        static const char* xyz_in_right;

        static const char* xyz_out;
        static const char* xyz_out_top;
        static const char* xyz_out_bottom;
        static const char* xyz_out_left;
        static const char* xyz_out_right;

        static const char* ov_edge;
        static const char* ov_edge_value;
        static const char* ov_edge_remove;

        static const char* enlarge;
        static const char* enlarge_before;
        static const char* enlarge_after;

    };

    //#################################################################################################################################################

    struct ZFS_API FilterSetupParams
    {
        int type{}; ///< data type
        scannertyp_t model{ TYP_UNDEF };
        bool profiler{}; ///< profiler or image in profiler mode
        bool hasAdditionalByte{}; ///< additional byte with filterinfo
        bool threeLine{ true }; ///< should be always on
        bool disableCompensator{ false };
        bool extensions{ true }; ///< use extended settings as well

        std::string info; ///< additional collected info for showing to the user
        std::string serial; ///< just info about the serial number of the device
        std::string device; ///< the device model name

        FilterSetupParams() = default;
        FilterSetupParams(int t, bool p = false, bool a = false, bool l = true) : type(t), profiler(p), hasAdditionalByte(a), threeLine(l) {}

        void setup(const ZFSHeader*);
    };

    //#################################################################################################################################################

    /**
    \brief a single filter parameter item
    */
    struct ZFS_API FilterParamItem
    {
        friend class FilterParams;

        enum type_t { T_UNDEF, T_DOUBLE, T_INT, T_BOOL };

        FilterParamItem() : idx(0), type(T_UNDEF) {}

        FilterParamItem(type_t t_, const std::string &key_, const std::wstring &unit_,
            double def_, double min_, double max_,
            filterID_t id_,
            const std::string &parKey_ = "")
            : idx(0),
              type(t_),
              filterID(id_),
              defVal(def_),
              minVal(min_),
              maxVal(max_),
              key(key_),
              desc(L""),
              unit(unit_)
        {
            value = def_;
            if (parKey_ != key_)
                subnode = parKey_;
        }

        // info about the evaluation
        enum class eval_t : int
        {
            EV_UNDEFINED,
            EV_SINGLE, // just on the pixel (intensity, range)
            EV_NACHBAR, // point the the point before and afterwards
            EV_LINE, // whole line (bad line)
            EV_MATRIX, // need 3 lines 3x3 matrix
            EV_LINES, // needs 3 lines completly
            EV_LINEHEADER,
        };


        /// return item type
        type_t getType() const { return type; }
        bool isType(type_t t) const { return type == t; }


        /// return a string representation of the value
        std::string getValue() const;
        void setValue(const std::string &neu);

        void setupLanguageDepended(const std::wstring& description, const std::wstring& h, const std::wstring& supp = std::wstring()) { desc = description, help = h; supported = supp;  }

        // add additional info like since version
        FilterParamItem& setupSupported(const std::wstring& supp) { supported = supp; return *this; }

        FilterParamItem& setupMask(uint64_t msk) { mask = msk; return *this; }

        FilterParamItem& setupEvaluate(eval_t e) { eval=e; return *this; }

        // check if mask is set
        bool isMaskSet(uint64_t m) const { return mask & m; }
        uint64_t getMask() const { return mask; }


        /// if this is a node with subitems
        bool isNode() const { return subnode.length() == 0; }

        std::string getKey() const { return key; }
        std::wstring getDesc() const;
        std::wstring getUnit() const;
        std::wstring getHelp() const;
        std::wstring getSupported() const { return supported; }
        std::string getMin() const;
        std::string getMax() const;
        std::string getDefault() const;
        filterID_t getFilterID() const { return filterID; }

        // check whether hide or not
        bool isVisible(bool advanced=true) const;

        static std::wstring eval2string(eval_t);

    public: // todo protected
        int idx{}; ///< incremented number while adding items
        type_t type{}; ///< the type of the item
        std::string subnode; ///< if it is a node
        double value{};        ///< the value
        filterID_t filterID{};

        double defVal{}; ///< default
        double minVal{}; ///< min
        double maxVal{}; ///< max

        enum mask_t : uint64_t
        {
            JUST_3LINE = 1,
            JUST_1LINE = 2,
            JUST_IMAGER = 4,
            JUST_PROFILER = 8,
            JUST_SLAM = 0x10,
            JUST_ADVANCED = 0x20,
            JUST_EXTENDED = 0x40,
        };


        std::string key;   ///< key for access
        std::string xml;   ///< key for xml file if different to key
        std::wstring desc; ///< string for dialog
        std::wstring unit; ///< unit, maybe empty
        std::wstring help; ///< detailed item description
        std::wstring supported; ///< information about version and type like "from 10.0" or just for 9012
        eval_t eval{eval_t::EV_UNDEFINED};
        bool visible{ true };
        uint64_t mask{};

    protected:
        std::string to_String(double val) const;
    };

    typedef std::map<std::string, FilterParamItem> filter_params_t;

    //#################################################################################################################################################

    /**
     * @brief Basic implementation for translating filter texts.
     *
     */
    class ZFS_API FilterTranslator {

    private:
        std::map<std::string, std::wstring> namesMap;
        std::map<std::string, std::wstring> helpMap;
        std::map<std::string, std::wstring> unitMap;

        void setupHelp();

    public:
        FilterTranslator() {
            setupHelp();
        }

        virtual ~FilterTranslator() {}

        // clear all translations
        void clear();

        /// set desc and help in once utf16 version
        void setTranslation(const std::string& key, const std::wstring& descUtf16, const std::wstring& helpUtf16 );
        /// set desc and help in once utf8 version
        void setTranslation(const std::string& key, const std::string& descUtf8, const std::string& helpUtf8 );


        /**
         * @brief Get the translated name
         *
         * @param item  item to get the translation for
         *
         * @return  the translated name
         */
        std::wstring getName(const FilterParamItem &item) 
        {
            return namesMap[item.getKey()];
        }

        /**
         * @brief Get the translated help text
         *
         * @param item  item to get the translation for
         *
         * @return  the translated help text
         */
        std::wstring getText(const FilterParamItem &item) 
        {
            return helpMap[item.getKey()];
        }
    };

    //#################################################################################################################################################

    /**
    \brief holds params for filtering
    */
    class ZFS_API FilterParams 
    {
    public:
        typedef std::function< void(FilterParams* prm, const FilterSetupParams& set) > extension_f;

    protected:
        static std::string language;    ///< the default language is en

        bool changed = false;

        FilterSetupParams setupPrm;
        filter_params_t prm; ///< the items map
        int currentIndexAdd{}; ///< current index for adding
        bool threeLine{ true };      ///< 3line filtering (default on)

        static std::shared_ptr<FilterTranslator> translator;
        static std::wstring templatePath;
        static std::vector< extension_f > extenstions;

    public:

        /// set a new language for the description and help text
        static void setLanguage(const std::string& l, const std::wstring& pathToLanguageFiles );

        // load a external language file into the translator
        static bool loadLanguageFile(const std::string& language, const std::wstring& pathToLanguageFiles);


        FilterParams();
        FilterParams(bool threeLineSupport);
        FilterParams(const ZFSHeader *, bool threeLineSupport = true);
        FilterParams(const ZFSHeader &, bool threeLineSupport = true);
        FilterParams(int typ, bool profiler, bool threeLineSupport =true);
        FilterParams(const std::wstring& zfs_fn, bool threeLineSupport = true);
        FilterParams(const FilterSetupParams& set);
        FilterParams(const std::shared_ptr<FilterParams> ); // create a copy

        virtual ~FilterParams() = default;

        // extension for filter
        // not thread save
        static void addExtenstion(extension_f);
        static void delExtenstion(extension_f);
        static void delExtenstions();

        /// An overloaded filter can extend the filter params. This will be called at the end of buildStandardFilterParams
        virtual void extendDefinitions(const FilterSetupParams&) {}

        FilterParams& buildStandardFilterParams(const FilterSetupParams& set);
        void buildStandardFilterParams(int typ, bool profiler = false, bool threeLine = true);
        void buildStandardFilterParams(const ZFSHeader &hd, bool threeLineSupport = true);
        void buildStandardFilterParams(const ZFSHeader* hd, bool threeLineSupport = true);
        bool buildStandardFilterParams(const std::wstring& zfs_fn, bool threeLineSupport = true);

        // set the visibility of the main nodes depending on device type
        void setVisibility( const FilterSetupParams& set);

        // for example set range limits for 5016
        void updateValuesDependingOnType();

        /// get real values like initensity or range limits from zfs-header
        void updateRealValuesFromHeader(const ZFSHeader &head);


        /// add a new definition to the filter param list
        FilterParamItem& addDefinition(const FilterParamItem &add);

        void updateTranslations(zfs::FilterParamItem* a);

        /// delete a filter param definition from the list as well as all subnodes
        void deleteDefinition(const std::string &key);

        // path where to find templates. They are used if scan did not have already params
        static void setTemplatePath(const std::wstring& path);
        // current template path defined
        static std::wstring getTemplatePath();

        /// load values from a ini file or template if exist
        // updateType can change datatype and profiler if type was 0
        bool loadFilterParams(const std::wstring &fileName, bool updateType=false, bool fallbackLoadTemplate=true );

        /// store values to a ini file
        bool storeFilterParams(const std::wstring &fileName) const;

        bool fromXML( const tinyxml2::XMLNode*, bool readType=false );
        bool toXML( tinyxml2::XMLNode* ) const;

        friend ZFS_API std::ostream &operator<<(std::ostream &os,
            const FilterParams &prm);

        /// fill ZFSFilter data structure with the params
        void setup(ZFSFilter &) const;
        void useFrom(const ZFSFilter &);

        const FilterParamItem *findViaKey(const std::string &key) const;

        FilterParamItem *findViaKey(const std::string &key);
        FilterParamItem *findViaDesc(const std::wstring &desc);

        std::vector<FilterParamItem *> getItemsForSubnode(const std::string &subnode);

            /// set value
        void setDouble(const std::string& key, double neu);
        void setInt(const std::string& key, int neu);
        void setBool(const std::string& key, bool neu);
        void setVisible(const std::string& key, bool vis);
        void setMask(const std::string& key, uint64_t mask, bool set=true);

        // define value. same as set value plus set the default to the same value
        void defineDefDouble(const std::string& key, double neu);
        void defineDefInt(const std::string& key, int neu);
        void defineDefBool(const std::string& key, bool neu);

        /// get value
        double getDouble(const std::string& key, double def = 0.) const;
        int getInt(const std::string& key, int def = 0) const;
        bool getBool(const std::string& key, bool def = false) const;
        uint64_t getMask(const std::string& key ) const;
        bool isMask(const std::string& key, uint64_t set, uint64_t notset) const;
        bool isVisible(const std::string& key, bool advanced=true ) const;

        std::string getAsString(const std::string &key, const std::string& def="" ) const;

        /// number of items and content must be the same
        bool isEqual(const FilterParams &p2) const;

        /// access
        const filter_params_t &items() const { return prm; }
        filter_params_t &items() { return prm; }
        size_t size() const { return prm.size();  }

        /// items sorted in the order they added
        std::vector<FilterParamItem *> itemsSorted();
        const std::vector<const FilterParamItem *> itemsSorted() const;

        /// return a sorted list with just the main nodes
        std::vector<FilterParamItem *> mainNodes();

        const FilterSetupParams& setupParams() const { return setupPrm; }
        int usedType() const { return setupPrm.type; }
        bool usedProfiler() const { return setupPrm.profiler; }
        bool used3Lines() const { return threeLine; }
        const std::string& getAdditionalInfo() const { return setupPrm.info; } ///< return additional info if set

        ///  set all values to its default value depending on device
        void resetDefaults(const std::string& subnode = "");

        // reset to template or to default if no template exist
        void reset2Template();

        /// switch all filters off, afterwards single filters can be switched on
        void allOff( bool intensityAsWell=true );

        /// checks wheter at least one filter is active
        bool isAnyFilterOn() const;

        // compare settings and return 0 if equal or number of differences
        int compare(const std::shared_ptr<FilterParams> ) const;

        /**
         * @brief Define the translator for this parameters.
         *        You can use this to set a specific translator from outside.
         *        If no translator is set, the FilterTranslator is used.
         *
         * @param _translator   the new translator
         */
        static void setTranslator();

        /**
         * @brief Get the current translator. Use the translator if you want to add
         *        translations dynamically or if you add parameters from outside via
         *        addDefinition().
         *
         * @return  the translator
         */
//        std::shared_ptr<FilterTranslator> getTranslator() const { return translator; }

        bool hasChanged() const;

        void setChanged(bool _changed);
    };

    //==============================================================================================

    ZFS_API void forceUseSigProc(bool en);
} // namespace zfs
