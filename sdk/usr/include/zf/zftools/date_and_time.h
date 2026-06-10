#pragma once

#include "zftools_api.h"

#include <string>
#include <chrono>

namespace zftools
{
    // ==============================================================================================
    enum class locale_t {UTC, LOCAL}; // local or utc time
    enum class date_and_time_format_t { DEFAULT, // "dd.mm.yyyy hh:mm:ss"
                                        Weekday, // "<weekday> dd.mm.yyyy hh:mm:ss"
                                        Weekday_TimeZone, // "<timezone> <weekday> dd.mm.yyyy hh:mm:ss"
                                        FilePostFixFormat // "_dd-mm-yyyy_hh_mm_ss"
                                      };

    std::string ZFTOOLS_API current_date_and_time(date_and_time_format_t date_and_time_format, locale_t locale = locale_t::LOCAL);
    // ==============================================================================================
    // UTC, "dd.mm.yyyy hh:mm:ss"
    std::string ZFTOOLS_API current_date_and_time(locale_t locale= locale_t::LOCAL);
    // ==============================================================================================
    // UTC, "<weekday> dd.mm.yyyy hh:mm:ss"
    std::string ZFTOOLS_API current_date_and_time_plus_weekday(locale_t locale = locale_t::LOCAL);
    // ==============================================================================================
    // UTC, "<timezone> <weekday> dd.mm.yyyy hh:mm:ss"
    std::string ZFTOOLS_API current_date_and_time_plus_weekday_and_timezone(locale_t locale = locale_t::LOCAL);
    // ==============================================================================================
    // UTC, "_dd-mm-yyyy_hh_mm_ss"
    std::string ZFTOOLS_API current_date_and_time_file_postfix(locale_t locale = locale_t::LOCAL);
    // ==============================================================================================
}