#include "vectorcorrectnesstest.h"

VectorCorrectnessTest::VectorCorrectnessTest()
{
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
