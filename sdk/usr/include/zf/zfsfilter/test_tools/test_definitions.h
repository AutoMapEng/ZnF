#include <string>

#include <zfsfilter/filterapi.h>

#include <zfcore/unicode.h>

namespace zfsfilter
{
    namespace test
    {
        using namespace std;

        // ==============================================================================================

        const wstring test_output_directory_WS = L"C:/temp";
        const wstring test_log_file_prefix_WS = L"log.";

        const string test_output_directory = zf::utf16_to_utf8(test_output_directory_WS);
        const string test_log_file_prefix = zf::utf16_to_utf8(test_log_file_prefix_WS);

        // ==============================================================================================
        FILTERDLL_API string generateTestLogFileName(const string& testName, const bool bAttachTime = true);
        // ==============================================================================================

    }
}