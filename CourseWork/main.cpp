#include <QCoreApplication>
#include "../Com_Lib/sf.h"
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include <iostream>
#include <QElapsedTimer>
#include <ctime>
#include "timetest.h"
#include "vectorcorrectnesstest.h"
#include "matrixcorrectnesstest.h"

void printVector(CBV& vect) {
    for (int i = 0; i < vect.GetBitLength(); i++) {
        cout << vect.GetBitAt(i);
    }
}

void printVector(CsBV& vect) {
    cout << "Bit length: " << vect.GetBitLength() << "\n";
    for (int i = 0; i < vect.GetBitLength(); i++) {
        cout << vect.GetBitAt(i);
    }
}

BOOL checkVectors(CBV vect, CsBV shortVector) {
    if (vect.GetBitLength() != shortVector.GetBitLength()) {
        return FALSE;
    }
    for (int i = 0; i < vect.GetBitLength(); i++) {
        if (vect.GetBitAt(i) != shortVector.GetBitAt(i)) {
            return FALSE;
        }
    }
    return TRUE;
}

void printMatrix(CsBM& matrix) {
    for (int i = 0; i < matrix.GetCountR(); i++) {
        for (int j = 0; j < matrix.GetCountC(); j++) {
            int bit = matrix.GetBitAt(i, j);
            cout << bit << " ";
        }
        cout << "\n";
    }
}

void printMatrix(CBM& matrix) {
    for (int i = 0; i < matrix.GetCountR(); i++) {
        for (int j = 0; j < matrix.GetCountC(); j++) {
            int bit = matrix.GetBitAt(i, j);
            cout << bit << " ";
        }
        cout << "\n";
    }
}

int** csbmToArray(CsBM& matrix) {
    int** array = new int*[matrix.GetCountR()];
    for (int i = 0; i < matrix.GetCountR(); i++) {
        array[i] = new int[matrix.GetCountC()];
    }
    for (int i = 0; i < matrix.GetCountR(); i++) {
        for (int j = 0; j < matrix.GetCountC(); j++) {
            int bit = matrix.GetBitAt(i, j);
            array[i][j] = bit;
        }
    }
}



BOOL checkEqual(CsBM& matrix, CBM& realMatrix) {
    for (int i = 0; i < matrix.GetCountR(); i++) {
        for (int j = 0; j < matrix.GetCountC(); j++) {
            int bit = matrix.GetBitAt(i, j);
            int realBit = realMatrix.GetBitAt(i, j);
            if (bit != realBit) {
                cout << "i = " << i << "; j = " << j << "\n";
                return FALSE;
            }
        }
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int repeatCount = 10000000;

    CBV realVector;
    realVector = realVector.GenRbv(32);
    cout << "Real vector:\n";
    printVector(realVector);
//    CsBV shortVector;
    CsBV *shortVector = new CsBV(31, FALSE);
    shortVector->ToShort(realVector);
    cout << "\nShort vector:\n";
    printVector(*shortVector);


    CBV realVector2;
    realVector2 = realVector2.GenRbv(32);
    cout << "\nReal vector2:\n";
    printVector(realVector2);
//    CsBV shortVector;
    CsBV *shortVector2 = new CsBV(31, FALSE);
    shortVector2->ToShort(realVector2);
    cout << "\nShort vector2:\n";
    printVector(*shortVector2);
    VectorCorrectnessTest vectorCorrectnessTest;

    cout << "\nCheck count bit: " << vectorCorrectnessTest.checkCountBit(realVector, *shortVector) << "\n";

    cout << "\nCheck left shift: " << vectorCorrectnessTest.checkLeftSHift(realVector, *shortVector, 5) << "\n";

    cout << "\nCheck right shift: " << vectorCorrectnessTest.checkRightSHift(realVector, *shortVector, 5) << "\n";

    cout << "\nCheck LeftOne: " << vectorCorrectnessTest.checkLeftOne(realVector, *shortVector, 5) << "\n";


    cout << BITS_COUNT << "\n\n";


    CBM realMatrix;
    realMatrix = realMatrix.GenRbm(64, 64);


    CsBM* shortMatrix = new CsBM(64, 64, FALSE);
    //shortMatrix->ToShort(realMatrix);

    MatrixCorrectnessTest matrixCorrectnessTest;


    cout << "\n\nMatrices are equal:   " << matrixCorrectnessTest.areEqual(realMatrix, *shortMatrix);
    for (int i = 0; i < realMatrix.GetCountR(); i++) {
        for (int j = 0; j < realMatrix.GetCountC(); j++) {
            shortMatrix->SetBitAt(i, j, realMatrix.GetBitAt(i, j));
        }
    }

    cout << "\n\nMatrices are equal:   " << matrixCorrectnessTest.areEqual(realMatrix, *shortMatrix);

    cout << "\n\nCheck transp: " << matrixCorrectnessTest.checkTransp(*shortMatrix);

    cout << "\n\nCheck bitwise exclusive: " << vectorCorrectnessTest.vectorsEqual(realMatrix.BitwiseExclusive(), shortMatrix->BitwiseExclusive());

    cout << "\n\nCheck count bit: " << matrixCorrectnessTest.checkCountBit(realMatrix, *shortMatrix);

    cout << "\n\nCheck disjunction: " << matrixCorrectnessTest.checkDisjunction(realMatrix, *shortMatrix);


    shortMatrix->ExchangeRow(3, 4);
    realMatrix.ExchangeRow(3, 4);
    cout << "\n\nAfter exchange row:   " << matrixCorrectnessTest.areEqual(realMatrix, *shortMatrix);

    CBM fromShort;
    shortMatrix->FromShort(fromShort);

    cout << "\n\nAfter from short - matrices are equal:   " << matrixCorrectnessTest.areEqual(fromShort, *shortMatrix);

    return a.exec();
}
