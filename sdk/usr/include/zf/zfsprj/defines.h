#pragma once

#include "file_extensions.h"

#define ZFPRJ_EXT			ext_zfprj
#define ZFPRJ_EXT_L			wext_zfprj

#define EXT_ZFPRJ_BACKUP	ext_zfprj_backup
#define BACKUP_FOLDER		folder_zfprj_backup
#define PRJ_LOCK_EXT		wext_zfprj_lock




/**
* Definitions for the export file extension
*/
#define PRJ_EXPORT_ASC_DEF "ASCII File (*.txt)"
#define PRJ_EXPORT_DXF_DEF "DXF File (*.dxf)"
#define PRJ_EXPORT_VRML_DEF "VRML File (*.wrl)"

#define REGXML_STR			"Register"
#define REGXML_DATA			"RegisterData"
#define REGXML_LEVEL		"Level"
#define REGXML_GPS			"GPS"
#define REGXML_COMPASS		"Compass"
#define REGXML_POSITIONS	"Positions"	// positionnsinfo waehrend des scanns
#define REGXML_POSITION 	"Position"
#define REGXML_TARGETS		"Targets"
#define REGXML_TARGET		"Target"
#define REGXML_TARGETDEFS	"TargetDefs"
#define REGXML_TACHY		"Tachy"

#define REGXML_PARAMS		"Params"
#define REGXML_PARAM		"Param"
#define REGXML_SCANS		"Scans"
#define REGXML_SCAN			"Scan"
#define REGXML_PRISMS		"Poss"
#define REGXML_PRISM		"Pos"
#define REGXML_MATRIX		"Matrix"
#define REGXML_FREEPOS      "FreePos"
#define REGXML_FREEPOS_CONNPOINTS  "ConnectionPoints"
#define REGXML_FREEPOS_CONNPOINT   "ConnectionPoint"
#define REGXML_ICP			"ICP"

// Attributes
#define REGXML_SCANPOS		"scanPos"

#define REGXML_ICP_VPNR     "viewPointNr"
#define REGXML_ICP_PREDECESSORNR     "predecessorNr"

#define REGXML_LEVEL_ERR    "error"
#define REGXML__USE			"use"
#define REGXML__NAME		"name"
#define REGXML__UNIT		"unit"
#define REGXML_QUAL			"qual"

#define REGXML_REGTYP           "registerType"             //Art der Registrierung die verwendet werden soll.
#define REGXML_REGFLAG			"registerFlag"
#define REGXML_COLOR			"color"
#define REGXML_FREEPOS_HEIGHT   "height"
#define REGXML_FREEPOS_X        "x"
#define REGXML_FREEPOS_Y        "y"
#define REGXML_FREEPOS_Z        "z"
#define REGXML_FREEPOS_NAME     "name"
#define REGXML_FREEPOS_TARGET_EXISTS "exist"
#define REGXML_FREEPOS_TARGET_LABEL  "label"

#define REGXML_ERRORNODE			"RegisterErrorProj"
#define REGXML_ERRORNODE_NR			"Nr"
#define REGXML_ERRORNODE_TEXT		"txt"

#define REGXML_VIEWPOINTS		    "Viewpoints"
#define REGXML_VIEWPOINT		    "Viewpoint"
#define REGXML_CONNECTIONS		    "Connections"
#define REGXML_CONNECTION		    "Connection"
#define REGXML_CONNECTION_TYP	    "connectionTyp"
#define REGXML_CONNECTION_AUTO	    "connectionAuto"
#define REGXML_CONNECTION_MATRIX	"ConnectionMatrix"
#define REGXML_CONNECTION_FLAG      "ConnectionFlag"
#define REGXML_CONNECTION_USE       "ConnectionUse"
#define REGXML_CONNECTION_P2P		"P2PInfo"
#define REGXML_FLOORPLAN		    "Floorplan"

#define REGXML_GLOBALMATRIX					"GlobalMatrix"
#define REGXML_COORDINATE_SYSTEMS           "CoordinateSystems"
#define REGXML_REGISTERDATA                 "RegisterData"
#define REGXML_REGISTERMATRIX				"RegisterMatrix"
#define REGXML_PREREGISTERMATRIX			"PreRegisterMatrix"
#define REGXML_DISTANCEMATRIX               "DistanceMatrix"
#define REGXML_COMPENSATOR					"Compensator"
#define REGXML_VERTICALAXISOBSERVATION		"VerticalAxisObservation"
#define REGXML_VERTICALAXISVARIANCEMATRIX	"VerticalAxisVarianceMatrix"
#define REGXML_VERTICALAXIS					"Verticalaxis"
#define REGXML_INCLINATION					"Inclination"
#define REGXML_TURNOVER_ERROR				"TurnoverError"
#define REGXML_LEVELMATRIX					"LevelMatrix"
#define REGXML_IMU                          "IMUValues"
#define REGXML_IMU_X                        "IMUValX"
#define REGXML_IMU_Y                        "IMUValY"

#define REGXML_NAVIGATION					"Navigation"
#define REGXML_COORDINATE                   "Coordinate"
#define REGXML_LATITUDE 					"latitude"
#define REGXML_LONGITUDE                    "longitude"
#define REGXML_SATELITES                    "satelites"
#define REGXML_HDOP                         "hdop"
#define REGXML_AGE                          "age"
#define REGXML_HEIGHT                       "height"
#define REGXML_VDOP                         "vdop"
#define REGXML_HACC                         "hacc"
#define REGXML_VACC                         "vacc"
#define REGXML_FLAGS                        "flags"
#define REGXML_UTC_POS                      "utc"

#define REGXML_UTM_COORDINATE               "UTMCoordinate"
#define REGXML_NORTH                        "north"
#define REGXML_EAST                         "east"
#define REGXML_ZONE                         "zone"
