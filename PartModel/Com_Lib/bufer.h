/////////////////////////////////////////////////////////////////
// Ver.1.1.1     03.12.2004   Russian == English
// Ver.1.1.2    17.03.2006   - поправки форматирования
// Ver.1.1.3    11.05.2006   - Кириенко
// Добавлены функции:
// GetLineFromStringArray, GetNextNameWithAdd, GetNextPartString
/////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <utility>

using namespace std;

typedef vector<string> stringarray;

int GetFileLine(FILE *in, string& buf, long& len, string& szComments);
int GetNextName(string& buf, int& pos, string& bufname);
int GetNextNameWithAdd(FILE *in, long& len, string& szComments,
                       string& buf, int& pos, string& bufname);
int GetNextPartString(FILE *in, long& len, string& szComments,
                      string& buf, int& pos, string& bufname);
int GetSimpleFileLine(FILE *in, string& buf, long& len);
string Trim(string Formula);
string BeginEndTrim(string Formula);
int GetNameVar(string& buf, int &pos, string& bufname);
int GetNamePrevVar(string& buf, int &pos, string& bufname);
string TrDosToWin(string buf);
int GetNextInpName(string& bufur, int& j, string& bufname);
int GetLineFromStringArray(stringarray &Obj, string& buf, int& index, string& szComments);
int GetNextNameWithAdd(stringarray &Obj, int& index, string& szComments,
                       string& buf, int& pos, string& bufname);
int GetNextPartString(stringarray &Obj, int& index, string& szComments,
                      string& buf, int& pos, string& bufname);
