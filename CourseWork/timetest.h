#ifndef TIMETEST_H
#define TIMETEST_H
#include "../Com_Lib/sf.h"
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include <QElapsedTimer>


class TimeTest
{
public:
    TimeTest();
    void testMatrices(int repeatCount, QElapsedTimer timer, int rowCount, int columnCount);
    void testVectors(int repeatCount, QElapsedTimer timer, int size);
};

#endif // TIMETEST_H
