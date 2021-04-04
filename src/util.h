#ifndef UTIL_H
#define UTIL_H

#include <istream>
#include <vector>

using namespace std;

// In util.cpp
extern int myStrNCmp(const string& s1, const string& s2, unsigned n);
extern size_t myStrGetTok(const string& str, string& tok, size_t pos = 0,
                          const string del = "\t \n");
extern bool myStr2Int(const string& str, int& num);
extern bool isValidVarName(const string& str);
//extern long long parseChar(char);

#endif // UTIL_H
