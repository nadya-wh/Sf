#include <QCoreApplication>
#include "../Com_Lib/sf.h"
#include <vector>
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CSF *sf1;
    sf1 = new CSF();
    sf1->ReadSFFromFile("C:/Users/User/Desktop/ag.sf");

    vector<string> v;

    sf1->WriteSF(v);

    for (int i = 0; i < v.size(); i++) {
        cout << v.at(i).c_str() << "\n";
    }

    return a.exec();
}
