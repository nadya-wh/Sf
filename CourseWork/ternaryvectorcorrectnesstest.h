#ifndef TERNARYVECTORCORRECTNESSTEST_H
#define TERNARYVECTORCORRECTNESSTEST_H

#include "../ComBool/ShortTern.h"
#include "../Common/BaseTern.h"

class TernaryVectorCorrectnessTest
{
public:
    TernaryVectorCorrectnessTest();
    BOOL areEqual(CTV realVector, CsTV shortVector);
    BOOL checkFromShort(int columnCount);
    BOOL checkInvertBits(int columnCount);
    void runTests(int columnCount);
};

#endif // TERNARYVECTORCORRECTNESSTEST_H
