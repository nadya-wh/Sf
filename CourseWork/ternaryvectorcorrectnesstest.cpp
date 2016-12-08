#include "ternaryvectorcorrectnesstest.h"

TernaryVectorCorrectnessTest::TernaryVectorCorrectnessTest()
{

}

BOOL TernaryVectorCorrectnessTest::areEqual(CTV realVector, CsTV shortVector) {
    for(int i = 0; i < realVector.GetBitLength(); i++) {
        if (realVector.GetBitAt(i) != shortVector.GetBitAt(i)) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL TernaryVectorCorrectnessTest::checkFromShort(int columnCount) {
    CsTV *ctv1 = new CsTV(columnCount);
    CTV ctv;
    ctv.GenRtv(columnCount);
    ctv1->FromShort(ctv);
    return areEqual(ctv, *ctv1);
}

BOOL TernaryVectorCorrectnessTest::checkInvertBits(int columnCount) {
    CsTV *ctv1 = new CsTV(columnCount);
    CTV ctv;
    ctv.GenRtv(columnCount);
    for(int i = 0; i < ctv.GetBitLength(); i++) {
        ctv1->SetBitAt(i, ctv.GetBitAt(i));
    }
    cout << "Equal: " <<areEqual(ctv, *ctv1);

    ctv.InvertBitsInPlace(TRUE);
    ctv1->InvertBitsInPlace(TRUE);

    return areEqual(ctv, *ctv1);
}


void TernaryVectorCorrectnessTest::runTests(int columnCount) {
    CTV ctv;
    ctv.GenRtv(columnCount);
    cout << "checkInvertBits: " << checkInvertBits(columnCount) << "\n";
    cout << "checkFromShort: " << checkFromShort(columnCount);
}
