/*
    QT based Sample of Ctrl
    Copyright by Zoller + Fröhlich GmbH

	Requires following libraries:
	- zfs
	- zfcore
	- lrcsocket

	since 10.0 You need now the zfs.dll and the zfcore.dll
*/


#include <qapplication.h>

#include "dialog/mainwindow.h"
#ifdef _WINDOWS
#include <zfcore/minidump.h>
#endif

#include "if.h"


//=======================================================================================================

int main( int argc, char* argv[] )
{
	// handler für minidump
#ifdef _WINDOWS
	zf::setAppInfo( L"SampleCtrlSocket", L"1.0", L"0", L"" );
	SetUnhandledExceptionFilter(zf::TopUnhandledExceptionFilter);
#endif

	QApplication qapp( argc, argv );


	MainWindow *mw = new MainWindow();

#ifdef USE_INTERFACE
	mw->myif = new zf::ctrl::MyLRC;
#endif

	mw->show();
    qapp.exec();

#ifdef USE_INTERFACE
	delete mw->myif;
#endif

	delete mw;
	return 0;
}

