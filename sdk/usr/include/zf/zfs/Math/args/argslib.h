#ifndef __ARGSLIB_H
#define __ARGSLIB_H

//***********************************************************************
// (c) Copyright 1999-2003 Santronics Software, Inc. All Rights Reserved.
//***********************************************************************
// File Name : argslib.h
// Subsystem : Command Line Parameters Wrapper
// Date      : 03/03/2003
// Author    : Hector Santos, Santronics Software, Inc.
// VERSION   : 1.00P
//
// Revision History:
// Version  Date      Author  Comments
// -------  --------  ------  -------------------------------------------
// v1.00P   03/03/03  HLS     Public Release version (non-SSL version)
//***********************************************************************

#include <string>

#include <zfs/zfsapi.h>

#define MAX_ARGS 100


class ZFS_API CommandLineParameters 
{
public:
    CommandLineParameters( const char* cmd, const char *switchchars = "-/");
    ~CommandLineParameters();

public:
	void reset(const char* cmd);
    bool CheckHelp(const bool bNoSwitches = false);

    int ParamCount() { return paramcount; }
//    std::string ParamLine();
//    std::string CommandLine();
    std::string ParamStr(int index, const bool bGetAll = false);
    int ParamInt(int index);

    int FirstNonSwitchIndex();
    std::string FirstNonSwitchStr();
    int SwitchCount();
    int Switch(const char *sz, const bool bCase = false);
    std::string GetSwitchStr(const char *sz, const char *szDefault = "", const bool bCase = false);
    int GetSwitchInt(const char *sz, const int iDefault = -1, const bool bCase = false);
    double GetSwitchDouble(const char *sz, const double iDefault = -1., const bool bCase = false);
    std::string GetNonSwitchStr(const bool bBreakAtSwitch = true, const bool bFirstOnly = false);


private:
    bool IsSwitch(const char *sz);
    const char *szSwitchChars;
    char *parms[MAX_ARGS];
    char *pszCmdLineDup;
    int maxparms;
    int paramcount;
};

#endif // __ARGSLIB_H