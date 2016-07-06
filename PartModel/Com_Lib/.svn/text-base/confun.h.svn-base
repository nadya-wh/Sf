#ifndef CONFUN
#define CONFUN

#include <vector>
#include <utility>
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"

typedef std::vector<CTM> MTM;
typedef std::pair <CTV, int> PairTv_I;
typedef std::vector <PairTv_I> MTv_I;
typedef std::vector <MTv_I> MMTv_I;

#include "../Common/Sop.h"
#include "../Common/Sbf.h"

#include "../Com_Lib/bufer.h"
#include "../Com_Lib/objects.h"
#include "../Com_Lib/sf.h"
#include "../Com_Lib/sflog.h"

#include "../ComBool/ShortBool.h"
#include "../ComBool/ShortTern.h"

int ConFunInSdf(string FileCon, string FileFun,
                 CBM *&pTrMat_Zero, CBM *&pTrMat_One, CBM *&pMatFun);
//BOOL ConFunInSdf(CString FileCon, CString FileFun, 
//                 CBM *&pTrMat_Zero, CBM *&pTrMat_One, CBM *&pMatFun);
int SdfInConFun(string FileCon, string FileFun,
                 CBM *pTrMat_Zero, CBM *pTrMat_One, CBM *pMatFun);
//BOOL SdfInConFun(CString FileCon, CString FileFun, 
//                 CBM *pTrMat_Zero, CBM *pTrMat_One, CBM *pMatFun);

#endif //CONFUN
