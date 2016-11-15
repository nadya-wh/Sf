#ifndef MATRIXCORRECTNESSTEST_H
#define MATRIXCORRECTNESSTEST_H

#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "vectorcorrectnesstest.h"

class MatrixCorrectnessTest
{
    VectorCorrectnessTest vectorCorrectnessTest;

public:
    MatrixCorrectnessTest();
    BOOL areEqual(CBM realMatrix, CsBM matrix);
    BOOL checkTransp (CsBM matrix);
    BOOL checkCountBit(CBM realMatrix, CsBM matrix);
    BOOL checkDisjunction(CBM realMatrix, CsBM matrix);



};

#endif // MATRIXCORRECTNESSTEST_H
