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

void MatrixCorrectnessTest::runTests(int rowCount, int columnCount) {
    CBM realMatrix;
    realMatrix = realMatrix.GenRbm(rowCount, columnCount);


    CsBM* shortMatrix = new CsBM(rowCount, columnCount, FALSE);
//    shortMatrix->ToShort(realMatrix);



    cout << "\n\nMatrices are equal:   " << areEqual(realMatrix, *shortMatrix);
    for (int i = 0; i < realMatrix.GetCountR(); i++) {
        for (int j = 0; j < realMatrix.GetCountC(); j++) {
            shortMatrix->SetBitAt(i, j, realMatrix.GetBitAt(i, j));
        }
    }

    cout << "\n\nMatrices are equal:   " << areEqual(realMatrix, *shortMatrix);

    cout << "\n\nCheck transp: " << checkTransp(*shortMatrix);

    cout << "\n\nCheck bitwise exclusive: " << vectorCorrectnessTest.vectorsEqual(realMatrix.BitwiseExclusive(), shortMatrix->BitwiseExclusive());

    cout << "\n\nCheck count bit: " << checkCountBit(realMatrix, *shortMatrix);

    cout << "\n\nCheck disjunction: " << checkDisjunction(realMatrix, *shortMatrix);


    shortMatrix->ExchangeRow(3, 4);
    realMatrix.ExchangeRow(3, 4);
    cout << "\n\nAfter exchange row:   " << areEqual(realMatrix, *shortMatrix);
}
