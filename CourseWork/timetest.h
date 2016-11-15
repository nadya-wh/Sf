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
    void testMatrices(int repeatCount, QElapsedTimer timer);
    void testVectors(int repeatCount, QElapsedTimer timer);
};

#endif // TIMETEST_H
