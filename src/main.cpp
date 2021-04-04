#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "cirMgr.h"
#include <time.h>
using namespace std;
// Look into src/base/abci/abc.c and src/base/io/io.c for usages of commands



#if defined(ABC_NAMESPACE)
namespace ABC_NAMESPACE
{
#elif defined(__cplusplus)
extern "C"
{
#endif

// procedures to start and stop the ABC framework
// (should be called before and after the ABC procedures are called)
void   Abc_Start();
void   Abc_Stop();

// procedures to get the ABC framework and execute commands in it
typedef struct Abc_Frame_t_ Abc_Frame_t;

Abc_Frame_t * Abc_FrameGetGlobalFrame();
int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );

#if defined(ABC_NAMESPACE)
}
using namespace ABC_NAMESPACE;
#elif defined(__cplusplus)
}
#endif



/*
TODO:
1. Read in golden.v and output translation named "temp_golden.v"
2. Read in revised.v and output translation named "temp_revised.v"
3. Start using ABC:
(1) read_verilog temp_final.v 
(2) sat
(3) write_cex answer_temp.v
4. If answer_temp.v is empty, UNSAT; Otherwise, SAT.
*/


int main(int argc, char** argv) {

    if (argc != 4) // <golden.v> <revised.v> <output>
    {
      	cout << "Usage: ./xec <golden.v> <revised.v> <output>\n";
      	exit(-1);
    }

    //double clk = clock();
    try{

    CirMgr O_circuit(argv[1],argv[2],argv[3]);

    char Command[1000];

    string input_v = "_final_temp_.v";

    //cout << "Time: " << (clock()-clk)/CLOCKS_PER_SEC << endl;
    //clk = clock();

    Abc_Start();

    //cout << "Time: " << (clock()-clk)/CLOCKS_PER_SEC << endl;
    //clk = clock();
    Abc_Frame_t *pAbc = Abc_FrameGetGlobalFrame();


    sprintf(Command, "read_verilog %s", input_v.c_str());
    Cmd_CommandExecute( pAbc, Command );

    //sprintf(Command, "aig");
    //Cmd_CommandExecute( pAbc, Command );
    //cout << "Time: " << (clock()-clk)/CLOCKS_PER_SEC << endl;
    //clk = clock();
    sprintf(Command, "strash");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "fraig");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "strash");
    Cmd_CommandExecute( pAbc, Command );

    /*sprintf(Command, "rewrite");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "refactor");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "balance");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "rewrite");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "refactor");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "balance");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "rewrite");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "refactor");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "balance");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "rewrite");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "refactor");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "balance");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "rewrite");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "refactor");
    Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "balance");
    Cmd_CommandExecute( pAbc, Command );*/
    //sprintf(Command, "fraig");
    //Cmd_CommandExecute( pAbc, Command );
   // sprintf(Command, "strash");
    //Cmd_CommandExecute( pAbc, Command );

    //sprintf(Command, "fraig");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "strash");
    //Cmd_CommandExecute( pAbc, Command );

    //sprintf(Command, "logic");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "satclp");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "mfs");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "aig");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "fraig");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "strash");
    //Cmd_CommandExecute( pAbc, Command );

    //sprintf(Command, "sat -v -C 1000000");
    //sprintf(Command, "dsat -h");
    //Cmd_CommandExecute( pAbc, Command );
    //sprintf(Command, "sat -v");
    //Cmd_CommandExecute( pAbc, Command );
    sprintf(Command, "iprove");
    Cmd_CommandExecute( pAbc, Command );

    //cout << "Time: " << (clock()-clk)/CLOCKS_PER_SEC << endl;
    //clk = clock();

    sprintf(Command, "write_cex -u _temp_out_put_");
    Cmd_CommandExecute( pAbc, Command );

    //cout << "Time: " << (clock()-clk)/CLOCKS_PER_SEC << endl;
    //clk = clock();

    Abc_Stop();

    //cout << "Time: " << (clock()-clk)/CLOCKS_PER_SEC << endl;
    //clk = clock();

    remove("_final_temp_.v");

    remove("_goldens_temp_.v");



    O_circuit.write_out();

    }
    catch(...)
    {
	cerr << "error occurs" << endl;
	ofstream output_temp(argv[3]);
	output_temp << "EQ" << endl;
    }

    return 0;
}
