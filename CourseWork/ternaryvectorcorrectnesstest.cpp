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

BOOL TernaryVectorCorrectnessTest::checkFromShort(int columnCount, CTV ctv2) {
        CsTV *ctv1 = new CsTV(columnCount);
        CTV ctv;
        ctv.GenRtv(columnCount);

        ctv1->FromShort(ctv);
    return areEqual(ctv, *ctv1);
}


void TernaryVectorCorrectnessTest::runTests(int columnCount) {
    CTV ctv;
    ctv.GenRtv(columnCount);
    cout << "Check FromShort: " << checkFromShort(columnCount, ctv);
}
