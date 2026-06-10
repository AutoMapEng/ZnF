#pragma once
#ifndef __ZFSPRJBASIC_H__
#define __ZFSPRJBASIC_H__
/**
* @brief Status for project plan items.
*/
enum ItemStatus {
    ITEM_OK = 1,
    ITEM_DELETED = 2,
    ITEM_INCOMPLETE = 3,
    ITEM_PLANED = 4
};

// Flag, ob "alte" Projekt-Attribute zusaetzlich in die Projektdatei eingefuegt werden soll
#define KEEP_DEPRECATED_VALUES

#define PROJECT_FILE_VERSION 1.7
// Aenderung 1.2:   Pregegister Matrix ist im Projekt nicht mehr vorhanden der Eintrag wird aber aus kompatibilitaetsgrunden mit den gleichen Werten wie die Registermatrix in der Projektdatei gefuert.
// Aenderung 1.3: (8.8)	Histogramm fuer Cloud to Cloud register info.
//						Fehlerwerte ViewPoint:mRegisterError hinzugefuegt
//						Neue Klasse PlaneToPlaneScoutRegisterInfo fuer das Speichern von scantra scout Ergebnissen hinzugekommen.
// Aenderung 1.4:   Flag fuer Multiscanner-Support hinzugefuegt
// Aenderung 1.5:   Flag fuer den Typ der Anzuwendenen Registrierung hinzugefuegt (ViewPoint)
//                  RegisterFlag aus dem Scan Eintrag geloescht wurde schon laenger nicht mehr verwendet.
// Aenderung 1.6:   CToCRegisterInfo neuer Parameter mConfidence
//                  ViewPoint::mConnectionDiff wird im Projekt gespeichert
//                  PlaneToPlaneRegisterInfo neue Parameter mUseKnownTarget und mOnlyTarget. 03.08.2017
//
// Aenderung 1.7:   Bereich fuer Projektplanung hinzugefuegt
//
///////////////////////////////////////////////

// Elementnamen fuer XML-Export eines Projekts
#define PRJXML_PROJECT		"Project"

#define PRJXML_SCANS		"Scans"
#define PRJXML_SCAN			"Scan"
#define PRJXML_CHILDS		"Childs"
#define PRJXML_CHILD		"Child"

#define PRJXML_VIEWS		"Views"
#define PRJXML_VIEW			"View"

#define PRJXML_OVERVIEW		"Overview"
#define PRJXML_GRID			"Grid"

#define PRJXML_LAYERS		"Layers"
#define PRJXML_LAYER		"Layer"
#define PRJXML_LINKS		"Links"
#define PRJXML_LINK			"Link"
#define PRJXML_LABELS		"Labels"
#define PRJXML_LABEL		"Label"

#define PRJXML_JOBS 		"Jobs"
#define PRJXML_JOB 		    "Job"

#define PRJXML_COMMENT 		"Comment"
#define PRJXML_OPERATOR     "Operator"
#define PRJXML_SITE         "Site"
#define PRJXML_COMPANY      "Company"

#define PRJXML_PLAN 		"Plan"
#define PRJXML_PLANITEM		"AnnotationItem"
#define PRJXML_ANNOTATION_VERSION "AnnotationVersion"
#define PRJXML_PLANITEMS	"AnnotationItems"
#define PRJXML_PLANGROUP    "AnnotationGroup"

#define PRJXML_FILTER      "FilterList"

#define PRJXML_SHOOT_CHANNEL	"ShootChannel"
#define PRJXML_VIEW_FROM_EYES	"ViewFromEye"

#define PRJXML_FORENSIC "Forensic"

#define PRJXML_SUSPECT_MEASURES "SuspectMeasures"
#define PRJXML_SUSPECT_MEASURE "SuspectMeasure"

#define PRJXML_CAMERA_CALIBRATIONS "CameraCalibrations"
#define PRJXML_CAMERA_CALIBRATION "CameraCalibration"

#define PRJXML_POSITION_ITEMS    "PositionItems"
#define PRJXML_DATA_ITEMS    "DataItems"

#define PRJXML_CAMERA_ITEMS "CameraItems"
#define PRJXML_CAMERA_ITEM  "CameraItem"

#define PRJXML_DATABASE_ITEMS "DatabaseItems"
#define PRJXML_DATABASE_ITEM  "DatabaseItem"

#define PRJXML_BOX_ITEMS    "BoxItems"
#define PRJXML_BOX_ITEM     "BoxItem"

#define PRJXML_ADDITIONAL_FILES_ITEMS    "AdditionalFiles"

#define PRJXML_MULTICLOUD_ITEMS    "Multiclouds"
#define PRJXML_MULTICLOUD_ITEM     "Multicloud"

#define PRJXML_SLAM_ITEMS           "SlamProject"
#define PRJXML_SLAM_RECORDS         "SlamRecords"
#define PRJXML_SLAM_RECORD          "SlamRecord"
#define PRJXML_SLAM_TARGET          "SlamTarget"
#define PRJXML_SLAM_TARGETS         "SlamTargets"
#define PRJXML_SLAM_AMNOTYPE         "annotationType"

#define PRJXML_SLAMRESULT_ITEMS     "SlamResults"
#define PRJXML_SLAMRESULT_ITEM      "SlamResult"
#define PRJXML_SLAMRESULT_CONNECTIONS "Connections"
#define PRJXML_SLAMRESULT_CONNECTION "Connection"
#define PRJXML_SLAMRESULT_TARGETS "Targets"
#define PRJXML_SLAMRESULT_TARGET "Target"

#define PRJXML_POINTS 		"Points"
#define PRJXML_POINT 		"Point"

#define PRJXML_PICTURES		"Pictures"
#define PRJXML_PICTURE		"Picture"


#define PRJXML_PARAMS		"Params"
#define PRJXML_PARAM		"Param"

#define PRJXML_GROUPS		"Groups"
#define PRJXML_GROUP		"Group"

#define PRJXML_GROUPS_REF	"GroupsRef"
#define PRJXML_GROUP_REF	"GroupRef"

#define PRJXML_METADATA         "Metadata"
#define PRJXML_METADATA_LIST    "MetadataList"

#define PRJXML_TAXONOMY    "taxonomy"

/////////////////////////////////////////////////////////////////////
// Attributnamen fuer XML-Export eines Projekts und enthaltene Items
// weitere Definitionen fuer Attribute bezogen auf Registrierung sind
// im Namensraum REGXML_ definiert
#define PRJXML_EXPORT		"export"
#define PRJXML_VERSION		"version"
#define PRJXML_MULTISCANNER "multiScanner"
#define PRJXML_SAVE_PROGRAM	"saveProgram"
#define PRJXML_OPEN			"open"
#define PRJXML_ID			"id"
#define PRJXML_UUID			"uuid"
#define PRJXML_NAME			"name"
#define PRJXML_PATH			"path"
#define PRJXML_FOLDER		"folder"
#define PRJXML_FILE			"file"
#define PRJXML_VISIBLE		"visible"
#define PRJXML_COLOR		"color"
#define PRJXML_ACTIVE		"active"
#define PRJXML_SELECT		"select"
#define PRJXML_TEXT			"text"
#define PRJXML_SCAN_TYP		"scanTyp"
#define PRJXML_TYP			"typ"
#define PRJXML_RESOLUTION	"resolution"
#define PRJXML_SCAN_ATTR	"scan"
#define PRJXML_PARENT		"parent"
#define PRJXML_MASTER		"master"
#define PRJXML_LAYER_ATTR	"layer"
#define PRJXML_POINT_COUNT	"nrPoints"
#define PRJXML_STATUS   	"status"
#define PRJXML_SCANNERNAME  "scannerName"
#define PRJXML_SCANDATE     "scanDate"

#define PRJXML_CAMERA		"camera"

#define PRJXML_U			"u"
#define PRJXML_V			"v"
#define PRJXML_X			"x"
#define PRJXML_Y			"y"
#define PRJXML_Z			"z"

#define PRJXML_X0			"x0"
#define PRJXML_Y0			"y0"
#define PRJXML_Z0			"z0"

#define PRJXML_X_SCAN			"scanX"
#define PRJXML_Y_SCAN			"scanY"
#define PRJXML_Z_SCAN			"scanZ"

#define PRJXML_UPSIDE_DOWN  "upsideDown"

#define PRJXML_STATISTIC    "statistic"
#define PRJXML_DISTVR_X      "distXVR"
#define PRJXML_DISTVR_Y      "distYVR"
#define PRJXML_DISTVR_Z      "distZVR"
#define PRJXML_DISTLC_X      "distXLC"
#define PRJXML_DISTLC_Y      "distYLC"
#define PRJXML_DISTLC_Z      "distZLC"
#define PRJXML_DISTLCKN_X    "distXKN"
#define PRJXML_DISTLCKN_Y    "distYKN"
#define PRJXML_DISTLCKN_Z    "distZKN"

#define PRJXML_MARGIN_X      "marginX"
#define PRJXML_MARGIN_Y      "marginY"
#define PRJXML_MARGIN_Z      "marginZ"
#define PRJXML_STDVAL        "stdValues"
#define PRJXML_STD           "stdValue"   
#define PRJXML_HIGHOFFSET    "offsetHight"
#define PRJXML_TIMESTAMP     "timestamp"

#define PRJXML_WIDTH		"width"
#define PRJXML_HEIGHT		"height"

#define PRJXML_SCALE_X		"scaleX"
#define PRJXML_SCALE_Y		"scaleY"
#define PRJXML_SCALE_Z		"scaleZ"

#define PRJXML_OPACITY		"opacity"
#define PRJXML_ROTATION		"rot"
#define PRJXML_RADIUS		"radius"
#define PRJXML_FLAG			"flag"

#ifdef _WINDOWS

# ifdef ZFSPRJ_EXPORTS
#  define ZFSPRJ_API __declspec(dllexport)
#  define ZFPRJ_API __declspec(dllexport)
# else
#  define ZFSPRJ_API __declspec(dllimport)
#  define ZFPRJ_API __declspec(dllimport)
# endif
#elif __GNUC__ >= 4
# define ZFSPRJ_API __attribute__ ((visibility ("default")))
# define ZFPRJ_API __attribute__ ((visibility ("default")))
#endif


#endif //__ZFSPRJBASIC_H__
