#include <QCoreApplication>
#include "../Com_Lib/sf.h"
#include "../ComBool/ShortBool.h"
#include "../ComBool/ShortTern.h"
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#include <iostream>
#include <QElapsedTimer>
#include <ctime>
#include "timetest.h"
#include "vectorcorrectnesstest.h"
#include "matrixcorrectnesstest.h"
#include "ternaryvectorcorrectnesstest.h"



//void printMatrix(CsBM& matrix) {
//    for (int i = 0; i < matrix.GetCountR(); i++) {
//        for (int j = 0; j < matrix.GetCountC(); j++) {
//            int bit = matrix.GetBitAt(i, j);
//            cout << bit << " ";
//        }
//        cout << "\n";
//    }
//}

//void printMatrix(CBM& matrix) {
//    for (int i = 0; i < matrix.GetCountR(); i++) {
//        for (int j = 0; j < matrix.GetCountC(); j++) {
//            int bit = matrix.GetBitAt(i, j);
//            cout << bit << " ";
//        }
//        cout << "\n";
//    }
//}




//BOOL checkEqual(CsBM& matrix, CBM& realMatrix) {
//    for (int i = 0; i < matrix.GetCountR(); i++) {
//        for (int j = 0; j < matrix.GetCountC(); j++) {
//            int bit = matrix.GetBitAt(i, j);
//            int realBit = realMatrix.GetBitAt(i, j);
//            if (bit != realBit) {
//                cout << "i = " << i << "; j = " << j << "\n";
//                return FALSE;
//            }
//        }
//    }
//    return TRUE;
//}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int repeatCount = 10000;
//    VectorCorrectnessTest vectorCorrectnessTest;
//    cout << "VECTORS TESTS for 32:\n\n";
//    vectorCorrectnessTest.runTests(32);
//    cout << "VECTORS TESTS for 64:\n\n";
//    vectorCorrectnessTest.runTests(64);
    MatrixCorrectnessTest matrixCorrectnessTest;
//    cout << "\n\nMATRICES TESTS for 32 and 32:\n\n";
//    matrixCorrectnessTest.runTests(32, 32);
//    cout << "\n\nMATRICES TESTS for 64 and 64:\n\n";
//    matrixCorrectnessTest.runTests(64, 64);
    cout << "\n\nTIME TESTS FOR SIZE = 31:\n";
    TimeTest timeTest;
    QElapsedTimer timer;
    timer.start();
//    timeTest.testVectors(repeatCount, timer, 31);
//    timeTest.testMatrices(repeatCount, timer, 31, 31);
//    timeTest.testTernaryVectors(repeatCount, timer, 31);
    timeTest.testTernaryMatrices(repeatCount, timer, 31, 31);

    cout << "\n\nTIME TESTS FOR SIZE = 63:\n";
//    timeTest.testVectors(repeatCount, timer, 63);
//    timeTest.testMatrices(repeatCount, timer, 63, 63);
//    timeTest.testTernaryVectors(repeatCount, timer, 63);
    timeTest.testTernaryMatrices(repeatCount, timer, 63, 63);

//    cout << "\n" << BITS_COUNT << "\n\n";

//    TernaryVectorCorrectnessTest tvCorrectnessTest;

//    //tvCorrectnessTest.runTests(63);

//    CsTV *ctv1 = new CsTV(63);
//    CTV ctv;
//    ctv.GenRtv(63);
//    for(int i = 0; i < ctv.GetBitLength(); i++) {
//        ctv1->SetBitAt(i, ctv.GetBitAt(i));
//    }

//    //ctv.

//    //ctv1->FromShort(ctv);

//    //cout << tvCorrectnessTest.areEqual(ctv, *ctv1);

//    for(int i = 0; i < ctv.GetBitLength(); i++) {
//        if (ctv.GetBitAt(i) != ctv1->GetBitAt(i)) {
//            cout << "FAIL! " << i << "\n";
//        }
//    }
    cout << "FINISH";


    return a.exec();
}
