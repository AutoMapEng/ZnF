#pragma once

#include <zfcore/string_cmdline.h>

namespace zfstr
{
    typedef zf::basic_cmdline<char> cmdline;
    typedef zf::basic_cmdline<wchar_t> wcmdline;
}


#if 0
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <deque>
#include <cctype>
#include <sstream>

namespace zfstr
{

/**
 * Search for the next word within str
 *
 * A word is delemited via space caracters. If it is quoted by '"' all
 * characters within '"' are threaded as a single word.
 *
 * @param str   The string to search for words
 * @param start The start of the search
 * @param [out] wstart The start of the word found or npos if non found
 * @param [out] wend  The end of the word or npos if word goes until end
 *                     of str
 * @return whether found word is quoted or not
 * @tparam CharT underlying character type
 */
template<class CharT>
bool getword(const std::basic_string<CharT> &str,
             typename std::basic_string<CharT>::size_type start,
             typename std::basic_string<CharT>::size_type &wstart,
             typename std::basic_string<CharT>::size_type &wend)
{
    bool quoted = false;
    wstart = str.find_first_not_of(' ', start);
    wend   = std::basic_string<CharT>::npos;

    if (wstart != std::basic_string<CharT>::npos)
        quoted = str[wstart] == '"';

    if (quoted)
        wend = str.find_first_of('"', ++wstart);
    else
        wend = str.find_first_of(' ', wstart);
    return quoted;
}

/**
 * @brief Parse command line
 *
 * Command line is splitted up into parameters + values and words
 *
 * example: -file "test file" -zip -opt1 1 -opt2 test   file2
 * result: -> params: file with value "test file"
 *                    zip
 *                    opt1 with value 1
 *                    opt2 with value test
 *            words:  file2
 *
 * Command separator defaults to '-' character but can be changed via
 * constructor parameter.
 *
 * @note Quoted strings are never threated as parameters they are always words!
 *
 * @note If case insensitivity is switched on (default), all parameters will be
 *       saved in lower case. Thus you have to request parameters in lower case!
 */
template<class T>
class basic_cmdline
{
    typedef std::map< std::basic_string<T>, std::basic_string<T> > params_t;
    typedef std::vector< std::basic_string<T> > words_t;
    typedef typename params_t::const_iterator params_it_t;
    typedef typename words_t::const_iterator words_it_t;

protected:
    std::basic_string<T> line; ///< the cmdline string
    bool casesensitive;        ///< should the object act case sensitive

    params_t paramlst;         ///< parameter to value map
    words_t  wordlst;          ///< word list

public:
    basic_cmdline()
        : casesensitive(false)
    {}

    basic_cmdline( const T* cmd, bool cs = false, T sep = '-' )
        : line(cmd),
          casesensitive(cs)
    {
        split(sep);
    }

    basic_cmdline( const std::basic_string<T>& cmd, bool cs = false, T sep = '-' )
        : line(cmd),
          casesensitive(cs)
    {
        split(sep);
    }

    virtual ~basic_cmdline() {}

    /**
     * Set commandline to cmd
     *
     * @param cmd commandline to parse
     * @param cs  should commands be handled case sensitive
     * @param sep command separator
     */
    void set( const std::basic_string<T> &cmd, bool cs = false, T sep = '-' )
    {
        paramlst.clear();
        wordlst.clear();
        line = cmd;
        casesensitive = cs;
        split(sep);
    }

    /**
     * Return the C equivalent of the initial command line
     * @return Null terminated C-String
     */
    const T* c_str() const
    {
        return line.c_str();
    }

    /**
     * Returns the c++ string
     * @return c++ string
     */
    const std::basic_string<T>& string() const
    {
        return line;
    }

    /**
     * Test if parameter is contained within command line
     *
     * @param key parameter to check
     * @return true if parameter is available
     */
    bool has( const std::basic_string<T>& key ) const
    {
        return paramlst.find(key) != paramlst.end();
    }

    /**
     * Return parameters string value
     *
     * @param key parameter value
     * @param def default value, taken if parameter isn't available
     * @return the parameters value or def
     */
    std::basic_string<T> str(
            const std::basic_string<T>& key,
            const std::basic_string<T>& def = std::basic_string<T>()) const
    {
        const params_it_t it = paramlst.find(key);
        if (it!=paramlst.end() && !it->second.empty())
            return it->second;

        return def;
    }

    /**
     * Returns the integer value of parameter key
     *
     * @param key parameter to return the value from
     * @param def default value if parameter is not available
     * @return def or the integer value of the parameter
     */
    int num(const std::basic_string<T> &key, int def = 0) const
    {
        const params_it_t it = paramlst.find(key);
        if (it == paramlst.end())
            return def;

        return lexical_cast<int>(it->second, def);
    }

    /**
     * Returns the double value of parameter key
     *
     * @param key parameter to return the value from
     * @param def default value if parameter is not available
     * @return def or the double value of the parameter
     */
    double dbl( const std::basic_string<T>& key, double def=0. ) const
    {
        const params_it_t it = paramlst.find(key);
        if (it == paramlst.end())
            return def;

        return lexical_cast<double>(it->second, def);
    }

    /**
     * Return the number of words within the command line
     *
     * @return number of words
     */
    int words() const
    {
        return static_cast<int>(wordlst.size());
    }

    /**
     * Return the word at position of command line
     *
     * @param idx Position of the word to retrieve
     * @return The word at position idx of the command line or "" if not available
     */
    std::basic_string<T> word( int idx ) const
    {
        if (idx < static_cast<int>(wordlst.size()))
            return wordlst[idx];
        return std::basic_string<T>();
    }

    /**
     * Return the number of parameter in command line
     * @return number of parameters
     */
    int params() const
    {
        return static_cast<int>(paramlst.size());
    }

    /**
     * Returns the parameter name of idx
     *
     * To receive the value use str()
     *
     * @return Parameter name at idx if it exists else an empty string
     */
    std::basic_string<T> param( int idx ) const
    {
        int i = 0;
        for(params_it_t it = paramlst.begin(); it != paramlst.end(); ++it)
        {
            if(i == idx)
                return it->first;
            ++i;
        }
        return std::basic_string<T>();
    }

    /**
    \brief revove the param with value from the param list
    */
    void removeParam(const std::basic_string<T>& key);

    /**
    \brief Rebuild the string using the words and params from the list
    \return new created string
    */
    std::basic_string<T> rebuild();

protected:

    /**
     * Does the separation of the command string into words or parameter + value
     *
     * @param sep separator to such as -
     */
    void split( const T sep )
    {
        // split up commandline in words
        typedef std::pair< bool, std::basic_string<T> > bool_str;
        std::deque< bool_str > wlist;
        typename std::basic_string<T>::size_type pos = 0;
        typename std::basic_string<T>::size_type wstart = 0;
        typename std::basic_string<T>::size_type wend = 0;
        while(pos != std::basic_string<T>::npos)
        {
            bool quoted = zfstr::getword(line, pos, wstart, wend);
            pos = wend;
            if(quoted && wend != std::basic_string<T>::npos)
                ++pos;

            if(wstart != std::basic_string<T>::npos)
            {
                wlist.push_back(bool_str(
                                    quoted,
                                    line.substr(wstart, wend - wstart)));
            }
        }

        // sort elements into words and params + their values
        typename std::deque<bool_str>::iterator it (wlist.begin());
        while(it != wlist.end())
        {
            if(isparam(it->second, sep) && !it->first)
            {
                std::basic_string<T> param(it->second, 1);
                if(!casesensitive)
                    std::transform(param.begin(), param.end(), param.begin(),
                                   tolower);

                ++it;
                if(it != wlist.end() && (!isparam(it->second, sep) || it->first))
                {
                    paramlst[param] = std::move(it->second);
                    ++it;
                }
                else
                    paramlst[param] = std::basic_string<T>();
            }
            else
            {
                wordlst.push_back(std::move(it->second));
                ++it;
            }
        }
    }

    /**
     * Checks wether str is a parameter name
     *
     * Parameters start with sep and are followed by one or more characters.
     *
     * @param str [in] String to check
     * @param sep      Parameter separator
     * @return true if str meets the constraints to be a parameter
     */
    bool isparam(const std::basic_string<T> &str, T sep)
    {
        return str.size() > 1 && str[0] == sep;
    }



private:

    /*
     * Helper functions
     * Special implementations of string to double and string to integer
     * convertion functions to make this class work as template class
     */

    template<typename Tout>
    Tout lexical_cast(const std::basic_string<T>& in, Tout def) const
    {
        std::basic_stringstream<T> ss;
        Tout res;

        ss << in;
        ss >> res;

        // failed to read formatted -> set default value
        if(!ss)
            res = def;
        return res;
    }
};

//================ inlines =======================================================================

template <class T> inline void basic_cmdline<T>::removeParam(const std::basic_string<T>& key)
{
    params_it_t it = paramlst.find(key);
    if (it != paramlst.end())
    {
        paramlst.erase(it);
    }
}

template <class T> inline std::basic_string<T> basic_cmdline<T>::rebuild()
{
    std::basic_string<T> s;

    for (auto itw = wordlst.begin(); itw != wordlst.end(); ++itw)
    {
        if (!s.empty())
            s.push_back(' ');
        s += *itw;
    }


    for (auto itp = paramlst.begin(); itp != paramlst.end(); ++itp)
    {
        if (!s.empty())
            s.push_back(' ');
        s.push_back('-');
        s += itp->first;
        if (!itp->second.empty())
        {
            s.push_back(' ');
            s.push_back('\"');
            s += itp->second;
            s.push_back('\"');
        }
    }

    return s;
}

//================================================================================================


typedef basic_cmdline<char> cmdline;
typedef basic_cmdline<wchar_t> wcmdline;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // end namespace zfstr

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif // ZFS_STRINGEX_H

