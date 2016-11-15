#ifndef VECTORCORRECTNESSTEST_H
#define VECTORCORRECTNESSTEST_H
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"


class VectorCorrectnessTest
{
public:
    VectorCorrectnessTest();
    BOOL vectorsEqual(CBV vect, CsBV shortVector);
    BOOL checkExtract(CBV vect, CsBV shortVector);

    BOOL checkConcat(CBV vect, CBV vectorToAdd, CsBV shortVector, CsBV shortVectorToAdd);
    BOOL checkLeftSHift(CBV vect, CsBV shortVector, int num);
    BOOL checkRightSHift(CBV vect, CsBV shortVector, int num);
    BOOL checkCountBit(CBV vect, CsBV shortVector);
    BOOL checkLeftOne(CBV vect, CsBV shortVector, int num);

};

#endif // VECTORCORRECTNESSTEST_H
