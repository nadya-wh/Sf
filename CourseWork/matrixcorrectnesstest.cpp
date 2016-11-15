#include "matrixcorrectnesstest.h"

MatrixCorrectnessTest::MatrixCorrectnessTest()
{
}

BOOL MatrixCorrectnessTest::areEqual(CBM realMatrix, CsBM matrix) {
    for (int i = 0; i < matrix.GetCountR(); i++) {
        for (int j = 0; j < matrix.GetCountC(); j++) {
            int bit = matrix.GetBitAt(i, j);
            int realBit = realMatrix.GetBitAt(i, j);
            if (bit != realBit) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL MatrixCorrectnessTest::checkTransp (CsBM matrix) {
    CsBM trans = matrix.Trans();
    for (int i = 0; i < matrix.GetCountR(); i++) {
        for (int j = 0; j < matrix.GetCountC(); j++) {
            int bit = matrix.GetBitAt(i, j);
            int transBit = trans.GetBitAt(j, i);
            if (bit != transBit) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL MatrixCorrectnessTest::checkCountBit(CBM realMatrix, CsBM matrix) {
    return areEqual(realMatrix, matrix) && realMatrix.CountBit() == matrix.CountBit();
}

BOOL MatrixCorrectnessTest::checkDisjunction(CBM realMatrix, CsBM matrix) {
    return areEqual(realMatrix, matrix) && vectorCorrectnessTest.vectorsEqual(realMatrix.Disjunction(), matrix.Disjunction());
}
