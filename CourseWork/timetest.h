#ifndef TIMETEST_H
#define TIMETEST_H
#include "../Com_Lib/sf.h"
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "../ComBool/ShortTern.h"
#include "../Common/BaseTern.h"
#include <QElapsedTimer>


class TimeTest
{
public:
    TimeTest();
    void testMatrices(int repeatCount, QElapsedTimer timer, int rowCount, int columnCount);
    void testVectors(int repeatCount, QElapsedTimer timer, int size);
    void testTernaryVectors(int repeatCount, QElapsedTimer timer, int size);
    void testTernaryMatrices(int repeatCount, QElapsedTimer timer, int rowCount, int columnCount);

};

#endif // TIMETEST_H
