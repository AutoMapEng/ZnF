#include <iostream>
#include <fstream>
#include <QString>
#include <zfsprj/zfprj.h>
#include <zfsprj/zfregisterobjnode.h>
#include <zfsprj/lockedprojectobjects.h>


using namespace std;

// ===================================================================================================================================

/*
 *  Main Function
 */

int main( int argc, char* argv[] )
{
    if (argc != 2)
    {
        cout << "usage: samplezfprj <projectfilename>" << endl;
        return -1;
    }

    // open the project
    std::wstring prjFileName = QString::fromLatin1(argv[1]).toStdWString();
    int err = 0;
    ZFProject prj( ZFProject::P_NOTHING);
    if (prj.Open(prjFileName, err))
    {
        // print superior coordinate system
        const ZFTRMatrix* globalMatrix = prj.GlobalMatrixPtr();
        if( globalMatrix )
        {
            double m[16];
            globalMatrix->GetTRMatrix4x4( m );
            cout << "global matrix" << endl;
            //Rotation                                                 //Translation
            cout << m[0]  << " " << m[1] <<  " " << m[2] <<  " " <<    m[3] << endl;
            cout << m[4]  << " " << m[5] <<  " " << m[6] <<  " " <<    m[7] << endl;
            cout << m[8]  << " " << m[9] <<  " " << m[10] << " " <<    m[11] << endl;
            cout << m[12] << " " << m[13] << " " << m[14] << " " <<    m[15] << endl;
        }
        cout << endl;

        // list all ZFS scans of the Project
        scans_list_t scans = prj.GetScansList();
        cout << "all scans" << endl;
        for (auto scan = scans.begin(); scan != scans.end(); ++scan)
        {
            ZFProjectItemScan* item = *scan;
            double value = item->Param("param", 1.23); // ask for a value stored in the xml stored for each scan
            cout << "value" << value << endl;
            // print the fullfilename
            cout << " " << QString::fromStdWString(item->FullFilename()).toLatin1().data() << endl;
        }
        cout << endl;


        zfprj::WritableLockedProjectObjects_Ptr<ZFRegisterObjNode> registerObj = prj.TryGetWriteableLockedObjects<ZFRegisterObjNode>();
        if( registerObj )
        {
            // list of known targets
            cout << "known targets:" << endl;
            for( auto knownTargets = registerObj->mKnownTargets.begin(); knownTargets != registerObj->mKnownTargets.end(); ++knownTargets )
                cout << ( *knownTargets ).NameC() << "   " << (*knownTargets).X() << " " << (*knownTargets).Y() << " " << (*knownTargets).Z() << endl;
            cout << endl;

            // list of all scan positions
            cout << "scan positions:" << endl;
            for( auto scanPositions = registerObj->mViewPointMap.begin(); scanPositions != registerObj->mViewPointMap.end(); ++scanPositions )
            {
                cout << "scan position:" << QString::fromStdWString( scanPositions->first ).toLatin1().data() << endl;

                ZFTRMatrix matScanPos = scanPositions->second->getCurrentMatrix(true);
                double m[16];
                matScanPos.GetTRMatrix4x4(m);
                cout << "matrix" << endl;
                //Rotation                                                 //Translation
                cout << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << endl;
                cout << m[4] << " " << m[5] << " " << m[6] << " " << m[7] << endl;
                cout << m[8] << " " << m[9] << " " << m[10] << " " << m[11] << endl;
                cout << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << endl;
                cout << endl;

                //master scan position
                if( scanPositions->second->isMaster() )
                    cout << "master scan psition" << endl;

                //registration status
                cout << "Scan position is registered: " << scanPositions->second->isRegisted() << endl;
                int regFlag = scanPositions->second->getRegFlag();
                cout << "Registaration typ: " << regFlag << " ";

                if( regFlag & 2 && !( regFlag & 0x20 ) )
                {
                    cout << "registered" << endl;
                }
                else  if( regFlag & 2 && regFlag & 0x20 && ( regFlag&0x100 || regFlag & 0x800 ) )
                {
                    cout << "pre registered" << endl;
                }
                else
                    cout << "not registered" << endl;
                cout << endl;

                // list of the scans in the scan position
                cout << " scans:" << endl;
                const std::list<GuardetZFProjectItemScan>& scanList = scanPositions->second->getScanList();
                for( auto scan = scanList.begin(); scan != scanList.end(); ++scan )
                {
                    if( scan->getPtr() )
                    {
                        cout << "   " << QString::fromStdWString( scan->getPtr()->Filename() ).toLatin1().data() << endl;
                    }
                }
                cout << endl;

                // list of the connections in the scan position
                cout << "  connections to:" << endl;
                std::list<RegisterVPConnection>* connectionList = scanPositions->second->getConnectionList();
                if( connectionList )
                {
                    for( auto connection = connectionList->begin(); connection != connectionList->end(); ++connection )
                    {
                        cout << "   " << QString::fromStdWString( ( *connection ).mReverdVP.c_str()).toLatin1().data() << endl;
                    }
                    cout << endl;
                }
                cout << endl;

                // list of the back connections to the scan position
                cout << "  connected from:" << endl;
                const std::list<std::wstring>* backConnections = scanPositions->second->getBackConnectionList();
                if( backConnections )
                {
                    for( auto backConnection = backConnections->begin(); backConnection != backConnections->end(); ++backConnection )
                    {
                        cout << "   " << QString::fromStdWString( ( *backConnection ).c_str() ).toLatin1().data() << endl;
                    }
                    cout << endl;
                }

                // list of the targets in the scan position
                cout << "  targets:" << endl;
                for( auto target = scanPositions->second->mTargets.begin(); target != scanPositions->second->mTargets.end(); ++target )
                    cout << "   " << ( *target ).NameC() <<  "   " << (*target).X() << " " << (*target).Y() << " " << (*target).Z() << endl;
                cout << endl;

            }
        }
    }
    return 0;
}
