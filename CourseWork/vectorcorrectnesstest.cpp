#include "vectorcorrectnesstest.h"

VectorCorrectnessTest::VectorCorrectnessTest()
{
}


void VectorCorrectnessTest::printVector(CBV& vect) {
    for (int i = 0; i < vect.GetBitLength(); i++) {
        cout << vect.GetBitAt(i);
    }
}

void VectorCorrectnessTest::printVector(CsBV& vect) {
    cout << "Bit length: " << vect.GetBitLength() << "\n";
    for (int i = 0; i < vect.GetBitLength(); i++) {
        cout << vect.GetBitAt(i);
    }
}


BOOL VectorCorrectnessTest::vectorsEqual(CBV vect, CsBV shortVector) {
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

BOOL VectorCorrectnessTest::checkConcat(CBV vect, CBV vectorToAdd, CsBV shortVector, CsBV shortVectorToAdd) {

    if (!vectorsEqual(vect, shortVector) || !vectorsEqual(vectorToAdd, shortVectorToAdd)) {
        return FALSE;
    }
    vect.Concat(vectorToAdd);
    shortVector.Concat(shortVectorToAdd);
    cout << "\n\nConcat:\n";
    for (int i = 0; i < 64; i++) {
        cout << vect.GetBitAt(i);
    }
    cout << "\n\n";
    for (int i = 0; i < 64; i++) {
        cout << shortVector.GetBitAt(i);
    }
    cout << "\n\n";
    return vectorsEqual(vect, shortVector);
}


BOOL VectorCorrectnessTest::checkLeftSHift(CBV vect, CsBV shortVector, int num) {

    if (!vectorsEqual(vect, shortVector)) {
        return FALSE;
    }

    vect = vect << num;
    shortVector = shortVector << num;
    if (!vectorsEqual(vect, shortVector)) {
        return FALSE;
    }

    return TRUE;
}

BOOL VectorCorrectnessTest::checkRightSHift(CBV vect, CsBV shortVector, int num) {

    if (!vectorsEqual(vect, shortVector)) {
        return FALSE;
    }

    vect = vect >> num;
    shortVector = shortVector >> num;
    if (!vectorsEqual(vect, shortVector)) {
        return FALSE;
    }

    return TRUE;
}


BOOL VectorCorrectnessTest::checkCountBit(CBV vect, CsBV shortVector) {
    return vectorsEqual(vect, shortVector) && vect.CountBit() == shortVector.CountBit();
}


BOOL VectorCorrectnessTest::checkLeftOne(CBV vect, CsBV shortVector, int num) {
    return vectorsEqual(vect, shortVector) && vect.LeftOne(num) == shortVector.LeftOne(num);
}


void VectorCorrectnessTest::runTests(int len) {
    CBV realVector;
    realVector = realVector.GenRbv(len);
    cout << "Real vector:\n";
    printVector(realVector);
    CsBV *shortVector = new CsBV(len, FALSE);
    shortVector->ToShort(realVector);
    cout << "\nShort vector:\n";
    printVector(*shortVector);

    CBV realVector2;
    realVector2 = realVector2.GenRbv(len);
    cout << "\nReal vector2:\n";
    printVector(realVector2);
    CsBV *shortVector2 = new CsBV(len, FALSE);
    shortVector2->ToShort(realVector2);
    cout << "\nShort vector2:\n";
    printVector(*shortVector2);

    cout << "\nCheck count bit: " << checkCountBit(realVector, *shortVector) << "\n";

    cout << "\nCheck left shift: " << checkLeftSHift(realVector, *shortVector, 5) << "\n";

    cout << "\nCheck right shift: " << checkRightSHift(realVector, *shortVector, 5) << "\n";

    cout << "\nCheck LeftOne: " << checkLeftOne(realVector, *shortVector, 5) << "\n";

}
