#ifndef __TCPCMD_H__
#define __TCPCMD_H__

//Einstellungen fuer Scan
#define TCP_FILE		"f"
#define TCP_PATH		"p"
#define TCP_HF1			"hf1"
#define TCP_HT1			"ht1"
#define TCP_HF2			"hf2"
#define TCP_HT2			"ht2"
#define TCP_HINC		"hi"
#define TCP_SPEED		"s"
#define TCP_LF			"lf"
#define TCP_LT			"lt"
#define TCP_LINES		"l"
#define TCP_COMPRESS	"c"
#define TCP_RANGE		"r"
#define TCP_FILTER		"fi"
#define TCP_FILESIZE	"fs"
#define TCP_CLKSOURCE	"cs"
#define TCP_CLKINTERN	"ci"
#define TCP_TRIGGER		"t"
#define TCP_MODE		"m"
#define TCP_WAIT		"wait"


#define LIFE_NOSCANNER	0  //kein Scanner angeschlossen
#define LIFE_SCANNEROK	1  //Scanner ok, laeuft nicht
#define LIFE_SCANNING	2  //momentan wird Scan aufgenommen
#define LIFE_DISCONNECTED	3  //keine Verbindung
#define LIFE_LISTENING	4	//Redundanz-PC hoert mit
#define LIFE_STARTING	5	//Scan wird vorbereitet
#define LIFE_STOPPING	6	//Scan wird gestoppt
#define LIFE_PAUSE		7   //Scan gestoppt, Spindeln drehen, Schlitten drin, TAcho wird weitergezaehlt

//Festlegen der Aufgabe jedes PCs im Netzwerk fuer ILAR
#define TASK_UNKNOWN 0
#define TASK_UNUSED 1 //PC hatte Verbindung verloren, wird jetzt nicht mehr benutzt
#define TASK_RED	2 //PC ist Redundanz-PC
#define TASK_MASTER 3 //steuert motorcontroller an
#define TASK_SCAN	4 //tut nichts ausser scannen auf befehl


#endif //__TCPCMD_H__
