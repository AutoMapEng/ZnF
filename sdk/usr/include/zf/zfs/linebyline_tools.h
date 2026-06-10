#pragma once
#include "zfsapi.h"
#include <vector>
#include <string>
#include <stdint.h>

class ZFSLinebyline;


namespace zfs
{
    /**
    \brief individual information about the part
    */
    struct ZFS_API Part
    {
        Part() : m_part(0), m_firstAbsLine(0), m_lastAbsLine(0) {}

        /// the current part index 0..
        uint32_t part() const { return m_part; }

        // return the first absolute line
        uint32_t firstAbsLine() const { return m_firstAbsLine; }

        // return the first absolute line
        uint32_t lastAbsLine() const { return m_lastAbsLine; }

        /// lines in this part
        uint32_t lines() const { return m_firstAbsLine - m_firstAbsLine + 1; }

        /// first relative line (always zero)
        uint32_t firstRelLine() const { return 0; }

        /// last relative line index
        uint32_t lastRelLine() const { return m_lastAbsLine - m_firstAbsLine; }

        const std::wstring& filename() const { return m_filename; }

        uint32_t m_part;
        uint32_t m_firstAbsLine;
        uint32_t m_lastAbsLine;
        std::wstring m_filename;

    };

    /**
    \brief collect some information about the scan if the scan is splitted in several parts
    */
    std::vector< Part > ZFS_API collectPartInfo(ZFSLinebyline*);
}