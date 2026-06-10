#pragma once

/**
\brief  The ports used by z+f software
*/

#define PORT_COMMAND        6100    // TCP
#define PORT_LINEDATA       6101    // UDP
#define PORT_STATUSDATA     6102    // UDP
#define PORT_STATUSDATA3    6103    // UDP
#define PORT_XYZTCP         6105    // TCP for linedata xyz or tcpzip
#define PORT_TCPZIP         PORT_XYZTCP
#define PORT_NAVIGATION     6106    // UDP navigation sensors
#define PORT_PROGRESS       6107    // progress and message/info

#define PORT_LRC            6120    // TCP
#define PORT_TRACE          6130    // UDP

#define PORT_NETSTORAGE_STATUS		6131

#define PORT_COMMAND_MOBILEMAPPING	6140	// command interface for controlling z+f-mobilemapping

#define PORT_TEMPSCHARNKSERVER_1  6171
#define PORT_TEMPSCHARNKSERVER_2  6172
#define PORT_TEMPSCHARNKSERVER_3  6173
#define PORT_MULTISCANNER   6180    // TCP
#define PORT_GPS_SIMULATOR  6190    // UDP
#define PORT_NAV_SIMULATOR  6191    // UDP

#define PORT_ZFSERVER	6400 // just Z+F internal
