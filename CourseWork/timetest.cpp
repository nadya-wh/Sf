#include "timetest.h"
#include <iostream>


TimeTest::TimeTest()
{
}


void testCBVVectorsOr(int repeatCount, QElapsedTimer timer, CBV bv1, CBV bv2) {
    timer.restart();
    CBV bv_1;
    for (int i=0; i<repeatCount; i++) {
        bv_1 = bv1 | bv2;
    }
    cout << "Elapsed time for 'CBV bv_1 = bv1 | bv2;'' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCsBVVectorsOr(int repeatCount, QElapsedTimer timer, CsBV sbv1, CsBV sbv2) {
    CsBV sbv_1;
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        sbv_1 = sbv1 | sbv2;
    }
    cout << "Elapsed time for 'CsBV sbv_1 = sbv1 | sbv2;': " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCBVVectorsMoveLeft(int repeatCount, QElapsedTimer timer, CBV bv_1) {
    timer.restart();
    CBV bv11;
    for (int i=0; i<repeatCount; i++) {
        bv11 = bv_1 << 7;
    }
    cout << "Elapsed time for 'CBV bv1 = bv_1 << 7;' : " << timer.nsecsElapsed() / repeatCount<< "\n";

}

void testCsBVVectorsMoveLeft(int repeatCount, QElapsedTimer timer, CsBV sbv_1) {
    timer.restart();
    CsBV sbv11;
    for (int i=0; i<repeatCount; i++) {
        sbv11 = sbv_1 << 7;
    }
    cout << "Elapsed time for 'CsBV sbv11 = sbv_1 << 7;' : " << timer.nsecsElapsed() / repeatCount << "\n";

    cout << "\n";
}

void testCBVVectorsMoveRight(int repeatCount, QElapsedTimer timer, CBV bv_1) {
    timer.restart();
    CBV bv11;
    for (int i=0; i<repeatCount; i++) {
        bv11 = bv_1 >> 25;
    }
    cout << "Elapsed time for 'CBV bv1 = bv_1 >> 25;' : " << timer.nsecsElapsed() / repeatCount<< "\n";

}

void testCsBVVectorsMoveRight(int repeatCount, QElapsedTimer timer, CsBV sbv_1) {
    timer.restart();
    CsBV sbv11;
    for (int i=0; i<repeatCount; i++) {
        sbv11 = sbv_1 >> 25;
    }
    cout << "Elapsed time for 'CsBV sbv11 = sbv_1 >> 25;' : " << timer.nsecsElapsed() / repeatCount << "\n";

    cout << "\n";
}

void TimeTest::testVectors(int repeatCount, QElapsedTimer timer, int size) {
    cout << "For CBV:\n";
    CBV bv1;
    bv1.GenRbv(size);
    CBV bv2;
    bv2.GenRbv(size);
    testCBVVectorsOr(repeatCount, timer, bv1, bv2);
    testCBVVectorsMoveLeft(repeatCount, timer, bv1);
    testCBVVectorsMoveRight(repeatCount, timer, bv1);

    cout << "\n\nFor CsBV:\n";
    CsBV sbv1;
    sbv1.ToShort(bv1);
    CsBV sbv2;
    sbv2.ToShort(bv2);
    testCsBVVectorsOr(repeatCount, timer, sbv1, sbv2);
    testCsBVVectorsMoveLeft(repeatCount, timer, sbv1);
    testCsBVVectorsMoveRight(repeatCount, timer, sbv1);
}

void testCBMMatrixRowConcat(int repeatCount, QElapsedTimer timer, CBM bm1, CBM bm2) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        bm1.GetRowBv(0) & bm2.GetRow(1);
    }
    cout << "Elapsed time for 'bm1.GetRowBv(j) & bm2.GetRow(k);' : " << timer.nsecsElapsed() / repeatCount << "\n";

}

void testCsBMMatrixRowConcat(int repeatCount, QElapsedTimer timer, CsBM* sbm1, CsBM* sbm2) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        sbm1->GetRowBv(0) & sbm2->GetRow(1);
    }
    cout << "Elapsed time for 'sbm1.GetRowBv(0) & sbm2.GetRow(1);' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCBMMatrixDisjunction(int repeatCount, QElapsedTimer timer, CBM bm) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        bm.Disjunction();
    }
    cout << "Elapsed time for 'bm.Disjunction();' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCsBMMatrixDisjunction(int repeatCount, QElapsedTimer timer, CsBM* sbm) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        sbm->Disjunction();
    }
    cout << "Elapsed time for 'sbm.Disjunction();' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCBMMatrixExchangeRow(int repeatCount, QElapsedTimer timer, CBM bm, int j) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        bm.ExchangeRow(j, j + 1);
    }
    cout << "Elapsed time for 'bm.ExchangeRow(j, j + 1);' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCsBMMatrixExchangeRow(int repeatCount, QElapsedTimer timer, CsBM* sbm, int j) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        sbm->ExchangeRow(j, j + 1);
    }
    cout << "Elapsed time for 'sbm.ExchangeRow(j, j + 1);' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCBMMatrixAdd(int repeatCount, QElapsedTimer timer, CBM bm) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        bm.Add(TRUE);
    }
    cout << "Elapsed time for 'bm.Add(TRUE);' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCsBMMatrixAdd(int repeatCount, QElapsedTimer timer, CsBM sbm) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        sbm.Add(TRUE);
    }
    cout << "Elapsed time for 'sbm.Add(TRUE);' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCBMMatrixSetRow(int repeatCount, QElapsedTimer timer, CBM bm1, CBM bm2, int _i, int j, int k) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        bm1.SetRow(j, bm2.GetRowBv(_i)- bm2.GetRow(k));
    }
    cout << "Elapsed time for 'bm1.SetRow(j, bm.GetRowBv(_i)- bm.GetRow(k));' : " << timer.nsecsElapsed() / repeatCount << "\n";

}

void testCsBMMatrixSetRow(int repeatCount, QElapsedTimer timer, CsBM* sbm1, CsBM* sbm2, int _i, int j, int k) {
    timer.restart();
    for (int i=0; i<repeatCount; i++) {
        sbm1->SetRow(j, sbm2->GetRowBv(_i)- sbm2->GetRow(k));
    }
    cout << "Elapsed time for 'sbm1.SetRow(j, sbm2.GetRowBv(_i)- sbm2.GetRow(k));' : " << timer.nsecsElapsed() / repeatCount << "\n";

}

void TimeTest::testMatrices(int repeatCount, QElapsedTimer timer, int rowCount, int columnCount) {
    CBM bm1;
    bm1 = bm1.GenRbm(rowCount, columnCount);
    CBM bm2;
    bm2 = bm2.GenRbm(rowCount, columnCount);

    // CBM
    cout << "For CBM matrices:\n";
    testCBMMatrixRowConcat(repeatCount, timer, bm1, bm2);
    testCBMMatrixDisjunction(repeatCount, timer, bm1);
    testCBMMatrixExchangeRow(repeatCount, timer, bm1, 1);
    //testCBMMatrixAdd(5, timer, bm1);
    testCBMMatrixSetRow(repeatCount, timer, bm1, bm2, 4, 5, 6);

    CsBM* sbm1 = new CsBM(rowCount, columnCount, FALSE);
    for (int i = 0; i < bm1.GetCountR(); i++) {
        for (int j = 0; j < bm1.GetCountC(); j++) {
            sbm1->SetBitAt(i, j, bm1.GetBitAt(i, j));
        }
    }
    CsBM* sbm2 = new CsBM(rowCount, columnCount, FALSE);
    for (int i = 0; i < bm2.GetCountR(); i++) {
        for (int j = 0; j < bm2.GetCountC(); j++) {
            sbm2->SetBitAt(i, j, bm2.GetBitAt(i, j));
        }
    }

    // CsBM
    cout << "\n\nFor CsBM matrices:\n";
    testCsBMMatrixRowConcat(repeatCount, timer, sbm1, sbm2);
    testCsBMMatrixDisjunction(repeatCount, timer, sbm1);
    testCsBMMatrixExchangeRow(repeatCount, timer, sbm1, 1);
    //testCsBMMatrixAdd(5, timer, sbm1);
    testCsBMMatrixSetRow(repeatCount, timer, sbm1, sbm2, 4, 5, 6);

}

void testCTMMatrixRowConcens(int repeatCount, QElapsedTimer timer, CTM tm1, CTM tm2) {
    timer.restart();

    for (int i=0; i<repeatCount; i++) {
        tm1.GetRowTv(0).Concens(tm2.GetRowTv(1));
    }
    cout << "Elapsed time for 'tm1.GetRowTv(0).Concens(tm2.GetRowTv(1));' : " << timer.nsecsElapsed() / repeatCount << "\n";

}

void testCsTMMatrixRowConcens(int repeatCount, QElapsedTimer timer, CsTM* stm1, CsTM* stm2) {
    timer.restart();

    for (int i=0; i<repeatCount; i++) {
        stm1->GetRowTv(0).Concens( stm2->GetRowTv(1));
    }
    cout << "Elapsed time for 'stm1->GetRowTv(0).Concens( stm2->GetRowTv(1));' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void testCTMMatrixOrthogon(int repeatCount, QElapsedTimer timer, CTM tm1) {
    timer.restart();

    for (int i=0; i<repeatCount; i++) {
        tm1.Orthogon(1, 2);
    }
    cout << "tm1.Orthogon(1, 2);' : " << timer.nsecsElapsed() / repeatCount << "\n";

}

void testCsTMMatrixOrthogon(int repeatCount, QElapsedTimer timer, CsTM* stm1) {
    timer.restart();

    for (int i=0; i<repeatCount; i++) {
        stm1->Orthogon(1, 2);
    }
    cout << "Elapsed time for 'stm1->Orthogon(1, 2);' : " << timer.nsecsElapsed() / repeatCount << "\n";
}

void TimeTest::testTernaryMatrices(int repeatCount, QElapsedTimer timer,
                                   int rowCount, int columnCount) {
    CTM bm1;
    bm1 = bm1.GenRtm(rowCount, columnCount);
    CTM bm2;
    bm2 = bm2.GenRtm(rowCount, columnCount);

    // CTM
    cout << "For CTM matrices:\n";
    testCTMMatrixRowConcens(repeatCount, timer, bm1, bm2);
    testCTMMatrixOrthogon(repeatCount, timer, bm1);

    CsTM* sbm1 = new CsTM(rowCount, columnCount, FALSE);
    for (int i = 0; i < bm1.GetCountR(); i++) {
        for (int j = 0; j < bm1.GetCountC(); j++) {
            sbm1->SetBitAt(i, j, bm1.GetBitAt(i, j));
        }
    }
    CsTM* sbm2 = new CsTM(rowCount, columnCount, FALSE);
    for (int i = 0; i < bm2.GetCountR(); i++) {
        for (int j = 0; j < bm2.GetCountC(); j++) {
            sbm2->SetBitAt(i, j, bm2.GetBitAt(i, j));
        }
    }

    // CsTM
    cout << "\n\nFor CsTM matrices:\n";
    testCsTMMatrixRowConcens(repeatCount, timer, sbm1, sbm2);
    testCsTMMatrixOrthogon(repeatCount, timer, sbm1);
}


void testCTVVectorsMoveLeft(int repeatCount, QElapsedTimer timer, CTV bv_1) {
    timer.restart();
    CTV bv11;
    for (int i=0; i<repeatCount; i++) {
        bv11 = bv_1 << 7;
    }
    cout << "Elapsed time for 'CTV tv1 = tv_1 << 7;' : " << timer.nsecsElapsed() / repeatCount<< "\n";

}

void testCsTVVectorsMoveLeft(int repeatCount, QElapsedTimer timer, CsTV sbv_1) {
    timer.restart();
    CsTV sbv11;
    for (int i=0; i<repeatCount; i++) {
        sbv11 = sbv_1 << 7;
    }
    cout << "Elapsed time for 'CsTV stv11 = stv_1 << 7;' : " << timer.nsecsElapsed() / repeatCount << "\n";

    cout << "\n";
}
void testCTVVectorsMoveRight(int repeatCount, QElapsedTimer timer, CTV bv_1) {
    timer.restart();
    CTV bv11;
    for (int i=0; i<repeatCount; i++) {
        bv11 = bv_1 >> 7;
    }
    cout << "Elapsed time for 'CTV tv1 = tv_1 >> 7;' : " << timer.nsecsElapsed() / repeatCount<< "\n";

}

void testCsTVVectorsMoveRight(int repeatCount, QElapsedTimer timer, CsTV sbv_1) {
    timer.restart();
    CsTV sbv11;
    for (int i=0; i<repeatCount; i++) {
        sbv11 = sbv_1 >> 7;
    }
    cout << "Elapsed time for 'CsTV stv11 = stv_1 >> 7;' : " << timer.nsecsElapsed() / repeatCount << "\n";

    cout << "\n";
}


void TimeTest::testTernaryVectors(int repeatCount, QElapsedTimer timer, int size) {
//    CsTV *ctv1 = new CsTV(63);
//    CTV ctv;
//    ctv.GenRtv(63);
//    for(int i = 0; i < ctv.GetBitLength(); i++) {
//        ctv1->SetBitAt(i, ctv.GetBitAt(i));
//    }

    cout << "For CTV:\n";
    CTV bv1;
    bv1.GenRtv(size);
    CTV bv2;
    bv2.GenRtv(size);
    testCTVVectorsMoveLeft(repeatCount, timer, bv1);
    testCTVVectorsMoveRight(repeatCount, timer, bv1);

    cout << "\n\nFor CsTV:\n";
    CsTV sbv1;
    sbv1.ToShort(bv1);
    CsTV sbv2;
    sbv2.ToShort(bv2);
    testCsTVVectorsMoveLeft(repeatCount, timer, sbv1);
    testCsTVVectorsMoveRight(repeatCount, timer, sbv1);



}




