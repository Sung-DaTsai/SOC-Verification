
#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirMgr
{
public:
   CirMgr(char* First, char* Second, char* output): output_file(output) { Netlist_construct(First, Second); }
   ~CirMgr() {  }
   void write_out() const;

private:
   string output_file;
   vector<string> PIid, POid;

   void Netlist_construct(const char*, const char*);
   void Parse_line(FILE*, FILE*);
   void Parse_line_R(FILE*, FILE*);
   void readPIs(size_t);
   void readPOs(size_t);
   void readwires_G(size_t, FILE*);
   void readwires_R(size_t, FILE*);
   void writemoduleinfo(FILE*, FILE*);
   void readgates(FILE*, bool=true);
   void writeDCgate(vector<string>&, vector<string>&, FILE*);
   void writeMUXgate(vector<string>&, vector<string>&, FILE*);
   void writeBUFgate(string&, vector<string>&, vector<string>&, FILE*);
   void writeNANDgate(string&, vector<string>&, vector<string>&, FILE*);
   void writeXORgate(string&, vector<string>&, vector<string>&, FILE*);
   void writeNOTgate(vector<string>&, vector<string>&, FILE*);
   void Write_output(FILE*, FILE*);


};

#endif // CIR_MGR_H
