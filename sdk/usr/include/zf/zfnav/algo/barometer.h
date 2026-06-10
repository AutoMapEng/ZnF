#pragma once
#include "../zfnav_api.h"


namespace navsens
{


//##################################################################################################################################
/**
 * @brief The Barometer class to handle all Barometeric/Heigth calculatinos.
 *
 */
class ZFNAV_API Barometer
{
public:

    double refBar{}; ///< to store refence pressure values [bar]
    double height{}; ///< to store refence height values [m]
    double alpha{};  ///< to store parameter for complimentray filter
    /**
     * @brief Barometer Constructor
     */
    Barometer();
    static Barometer *instance;
    static Barometer& getInstance();

    /**
     * @brief heightFilter a filter smoth height data
     * @param bar  current pressure value to be filtred [bar]
     * @return
     */
    void heightFilter(double bar);
    /**
     * @brief calcPressureAtSeaLevel Used to calculate baromteric pressure at sea level given the height
     * and current pressure. Useful function if height is arealdy availabel either from last scan position,
     * from GPS or any other source
     * @param bar Input pressure mreasured in pressure unit [bar]
     * @param height Input height mesareud in distance unit [m]
     * @return  Return refence presseure at sea level in pressure units [bar]
     */
    double calcPressureAtSeaLevel(double bar, double height);
    /**
     * @brief preassure2Height Convert Pressure to height measurements
     * @param bar Input pressure mreasured in pressure unit [bar]
     * @return  Return height at current level in distance units [m]
     */
    double preassure2Height(double bar) const;
    /**
     * @brief preassure2Height Convert Pressure to height measurements
     * @param bar Input pressure mreasured in pressure unit [bar]
     * @param refBar Reference baromter value in pressure units [bar]
     * @return  Return height at current level in distance units [m]
     */
    double preassure2Height(double bar, double refBar) const;
    /**
     * @brief getBarometricHeight to return the current filter height from the Barometer
     * @return filtered height [m]
     */
    double getBarometricHeight() const;
};

//##################################################################################################################################


}//end namespace
