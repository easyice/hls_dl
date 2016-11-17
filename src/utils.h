#ifndef UTILS_H
#define UTILS_H
#include <list>
#include <iostream>

using namespace std;


extern int findprocess ( list<string>& arrayfile,string dirname );

//输出不带换行符。除非输出与输入相同 
extern string removeparam(const string& filename);

extern void CreateMulPath( const char *muldir );

#endif
