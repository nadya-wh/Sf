#include <QCoreApplication>
#include "../Com_Lib/sf.h"
#include "../ComBool/ShortBool.h"
#include <iostream>
#include <QElapsedTimer>
#include <ctime>


//#ifndef _64_BITS_
//#define _64_BITS_
//#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    // data creation
    CBV bv1 = GetRandV();
    CBV bv2 = GetRandV();
    cout << "jj222\n";
    CsBV sbv1;
    sbv1.ToShort(bv1);
    CsBV sbv2;
    sbv2.ToShort(bv2);

    // tests
    QElapsedTimer timer;

    cout << "Bits: " << BITS_COUNT << "\n";
//1
    timer.start();
    CBV bv_1 = bv1 | bv2;
    cout << "Elapsed time for 'CBV bv_1 = bv1 | bv2;'' : " << timer.nsecsElapsed() << "\n";
    timer.restart();

    CsBV sbv_1;
    timer.restart();
    sbv_1 = sbv1 | sbv2;
    cout << "Elapsed time for 'CsBV sbv_1 = sbv1 | sbv2;': " << timer.nsecsElapsed()<< "\n";

    cout << "\n";
//2
    timer.restart();
    CBV bv11 = bv_1 << 7;
    cout << "Elapsed time for 'CBV bv1 = bv_1 << 7;' : " << timer.nsecsElapsed() << "\n";

    timer.restart();
    CsBV sbv11 = sbv_1 << 7;
    cout << "Elapsed time for 'CsBV sbv11 = sbv_1 << 7;' : " << timer.nsecsElapsed() << "\n";

    cout << "\n";
//3
    timer.restart();
    bv11 = bv2 >> 25;
    cout << "Elapsed time for 'bv = bv2 >> 25;' : " << timer.nsecsElapsed() << "\n";

    timer.restart();
    sbv11 = sbv_1 >> 25;
    cout << "Elapsed time for 'sbv11 = sbv_1 >> 25;' : " << timer.nsecsElapsed() << "\n";

    timer.restart();

    cout << "\nBits: " << BITS_COUNT << "\n";

    return a.exec();
}
