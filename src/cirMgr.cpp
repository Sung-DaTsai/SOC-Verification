#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <time.h>
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Implement member functions for class CirMgr

static string parse;
static bool parsing_state = false;
static size_t wire_num = 0; 

void
CirMgr::write_out() const
{
    ifstream input("sat_result_temp.out");
    input >> parse;
    input.close();
    remove("sat_result_temp.out");

    if (parse == "SAT")
    {
	ifstream input_answer("_temp_out_put_");
	input_answer >> parse;
	input_answer.close();
	remove("_temp_out_put_");
	FILE* out_put = fopen(output_file.c_str(), "w");
	fprintf(out_put, "NEQ\n");
   	for (unsigned i=0, n=PIid.size(); i<n; ++i)
   	{
	    fprintf(out_put, "%s %c\n", (PIid.at(i)).c_str(), parse[i]);
   	}
	fclose(out_put);
    }
    else
    {
	FILE* out_put = fopen(output_file.c_str(), "w");
	fprintf(out_put, "EQ");
	fclose(out_put);
    }
}



void
CirMgr::Netlist_construct(const char* golden_netlist, const char* revised_netlist)
{
    char* text, *temp;
    FILE *pf = fopen(golden_netlist, "r");
    FILE *finals = fopen("_final_temp_.v", "w");
    FILE *goldens = fopen("_goldens_temp_.v", "w");

    if (!pf)
    {
      	cerr << "Cannot open verilog file \"" << golden_netlist << "\"!!" << endl;
      	return;
    }

    // Start to parse golden file
    fseek(pf, 0, SEEK_END);
    long long lSize = ftell(pf);
    text = (char*)malloc(lSize+1);
    rewind(pf);
    fread(text, sizeof(char), lSize, pf);
    text[lSize] = '\0';
    temp = text;

    parsing_state = false;
    while (true)
    {
	parse = "";
	while (*temp != ';')
	{
	    bool check_again = false;
	    if (*temp == '\0')
		break;
	    else if ((*temp == '/') && (*(temp+1) == '*'))
	    {
		while ((*temp != '*') || (*(temp+1) != '/'))  *temp++;
		*temp++;
		*temp++;
		check_again = true;
	    }
	    else if ((*temp == '/') && (*(temp+1) == '/'))
	    {
		while (*temp != '\n')  *temp++;
		*temp++;
		check_again = true;
	    }
	    if (!check_again)
	        parse += *temp++;
	    else
		continue;
	}
	if (*temp == '\0')
	    break;
	parse += ';';
	Parse_line(finals, goldens);

	*temp++;
    }

    fclose(pf);

    free(text);

    char* text_r, *temp_r;
    FILE *pf_r = fopen(revised_netlist, "r");

    if (!pf_r)
    {
      	cerr << "Cannot open verilog file \"" << revised_netlist << "\"!!" << endl;
      	return;
    }

    // Start to parse revised file
    fseek(pf_r, 0, SEEK_END);
    lSize = ftell(pf_r);
    text_r = (char*)malloc(lSize+1);
    rewind(pf_r);
    fread(text_r, sizeof(char), lSize, pf_r);
    text_r[lSize] = '\0';
    temp_r = text_r;


    parsing_state = false;

    while (true)
    {
	parse = "";
	while (*temp_r != ';')
	{
	    bool check_again = false;
	    if (*temp_r == '\0')
		break;
	    else if ((*temp_r == '/') && (*(temp_r+1) == '*'))
	    {
		while ((*temp_r != '*') || (*(temp_r+1) != '/'))  *temp_r++;
		*temp_r++;
		*temp_r++;
		check_again = true;
	    }
	    else if ((*temp_r == '/') && (*(temp_r+1) == '/'))
	    {
		while (*temp_r != '\n')  *temp_r++;
		*temp_r++;
		check_again = true;
	    }
	    if (!check_again)
	        parse += *temp_r++;
	    else
		continue;
	}
	if (*temp_r == '\0')
	    break;
	parse += ';';
	Parse_line_R(finals, goldens);

	*temp_r++;
    }


    Write_output(finals, goldens);

    fprintf(goldens, "endmodule");

    fclose(pf_r);
    //fclose(finals);
    fclose(goldens);
    free(text_r);

    //FILE *finalss = fopen("_final_temp_.v", "a");
    FILE *goldenss = fopen("_goldens_temp_.v", "r");

    char str[500];
    while (fgets (str, 500, goldenss) != NULL)
	fputs(str, finals);

    fclose(goldenss);
    //fclose(finalss);
    fclose(finals);


}

void
CirMgr::Write_output(FILE* finals, FILE* goldens)
{
   for (vector<string>::const_iterator iter=POid.begin(); iter != POid.end(); iter++)
   {
	fprintf(goldens, "  not(wire_%lu, %s_r1 );\n", wire_num++, (*iter).c_str());
	fprintf(goldens, "  not(wire_%lu, %s_g2 );\n", wire_num++, (*iter).c_str());
	fprintf(goldens, "  and(wire_%lu, %s_g1 ", wire_num++, (*iter).c_str());
	fprintf(goldens, ", wire_%lu );\n", wire_num-3);
	fprintf(goldens, "  and(wire_%lu, %s_r2 ", wire_num++, (*iter).c_str());
	fprintf(goldens, ", wire_%lu);\n", wire_num-3);

	fprintf(goldens, "  or(wire_%lu", wire_num++);
	fprintf(goldens, "  , wire_%lu, wire_%lu);\n", wire_num-3, wire_num-2);
	fprintf(goldens, "  or(wire_%lu, %s_g1 ", wire_num++, (*iter).c_str());
	fprintf(goldens, ", wire_%lu);\n", wire_num-5);
	fprintf(goldens, "  and(wire_%lu", wire_num++);
	fprintf(goldens, "  , wire_%lu, wire_%lu);\n", wire_num-3, wire_num-2);
   }
   fprintf(goldens, "  or(final_output, ");
   for (unsigned i=0, n=POid.size()-1; i<n; ++i)
   {
	//fprintf(goldens, "wire_%lu, ", wire_num-1-4*i);
	//fprintf(goldens, "wire_%lu, ", wire_num-2-4*i);
	fprintf(goldens, "wire_%lu, ", wire_num-1-7*i);
   }
   //fprintf(goldens, "wire_%lu, ", wire_num-1-4*(POid.size()-1));
   //fprintf(goldens, "wire_%lu);\n", wire_num-2-4*(POid.size()-1));
   fprintf(goldens, "wire_%lu);\n", wire_num-1-7*(POid.size()-1));

   fprintf(finals, "wire ");
   for (size_t i=0; i<wire_num-1; ++i)
	fprintf(finals, "wire_%lu, ", i);
   fprintf(finals, "wire_%lu;\n", wire_num-1);
}


void
CirMgr::Parse_line(FILE* finals, FILE* goldens)
{
    string temp_str;
    size_t end = myStrGetTok(parse, temp_str);
    if (! parsing_state)
    {
	if ((temp_str[0] == 'm') && (temp_str[1] == 'o'))
	    return;
	else if ((temp_str[0] == 'i') && (temp_str[1] == 'n'))
	    readPIs(end);
	else if ((temp_str[0] == 'o') && (temp_str[1] == 'u'))
	    readPOs(end);
	else if ((temp_str[0] == 'w') && (temp_str[1] == 'i'))
	    readwires_G(end, goldens);
	else
	{
	    writemoduleinfo(finals, goldens);
	    readgates(goldens);
	    parsing_state = true;
	}
    }
    else
    {
	if ((temp_str[0] == 'e') && (temp_str[1] == 'n'))
	    return;
	readgates(goldens);
    }

}

void
CirMgr::Parse_line_R(FILE* finals, FILE* goldens)
{
    string temp_str;
    size_t end = myStrGetTok(parse, temp_str);
    if (! parsing_state)
    {
	if ((temp_str[0] == 'm') && (temp_str[1] == 'o'))
	    return;
	else if ((temp_str[0] == 'i') && (temp_str[1] == 'n'))
	    return;
	else if ((temp_str[0] == 'o') && (temp_str[1] == 'u'))
	    return;
	else if ((temp_str[0] == 'w') && (temp_str[1] == 'i'))
	    readwires_R(end, finals);
	else
	{
	    readgates(goldens, false);
	    parsing_state = true;
	}
    }
    else
    {
	if ((temp_str[0] == 'e') && (temp_str[1] == 'n'))
	    return;
	readgates(goldens, false);
    }
}

void
CirMgr::writemoduleinfo(FILE* finals, FILE* goldens)
{
   fprintf(finals, "module top(");
   for (vector<string>::const_iterator iter=PIid.begin(); iter != PIid.end(); iter++)
   {
	fprintf(finals, " %s_input , ", (*iter).c_str());
   }
   fprintf(finals, "final_output);\n");
   fprintf(finals, "input ");
   for (vector<string>::const_iterator iter=PIid.begin(); iter != PIid.end()-1; iter++)
   {
	fprintf(finals, " %s_input , ", (*iter).c_str());
   }
   fprintf(finals, " %s_input ;\n", (*(PIid.end()-1)).c_str());
   fprintf(finals, "output final_output;\n");
   fprintf(finals, "wire final_output, ");

   for (vector<string>::const_iterator iter=PIid.begin(); iter != PIid.end()-1; iter++)
   {
	fprintf(finals, " %s_input , %s_g1 , %s_g2 , %s_r1 , %s_r2 , ", (*iter).c_str(), (*iter).c_str(), (*iter).c_str(), (*iter).c_str(), (*iter).c_str());
	fprintf(goldens, "  buf( %s_r1 , %s_input );\n", (*iter).c_str(), (*iter).c_str());
	fprintf(goldens, "  buf( %s_g1 , %s_input );\n", (*iter).c_str(), (*iter).c_str());
	fprintf(goldens, "  buf( %s_r2 , %s_input );\n", (*iter).c_str(), (*iter).c_str());
	fprintf(goldens, "  buf( %s_g2 , %s_input );\n", (*iter).c_str(), (*iter).c_str());
   }
   fprintf(finals, " %s_input , %s_g1 , %s_g2 , %s_r1 , %s_r2  ;\n", (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str());
   fprintf(goldens, "  buf( %s_r1 , %s_input );\n", (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str());
   fprintf(goldens, "  buf( %s_g1 , %s_input );\n", (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str());
   fprintf(goldens, "  buf( %s_r2 , %s_input );\n", (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str());
   fprintf(goldens, "  buf( %s_g2 , %s_input );\n", (*(PIid.end()-1)).c_str(), (*(PIid.end()-1)).c_str());
   fprintf(finals, "wire ");
   for (vector<string>::const_iterator iter=POid.begin(); iter != POid.end()-1; iter++)
   {
	fprintf(finals, " %s_g1 , %s_g2 , %s_r1 , %s_r2 , ", (*iter).c_str(), (*iter).c_str(), (*iter).c_str(), (*iter).c_str());
   }
   fprintf(finals, " %s_g1 , %s_g2 , %s_r1 , %s_r2  ;\n", (*(POid.end()-1)).c_str(), (*(POid.end()-1)).c_str(), (*(POid.end()-1)).c_str(), (*(POid.end()-1)).c_str());
}

void
CirMgr::readPIs(size_t end)
{
   string temp_str;
   while (end != string::npos)
   {
	end = myStrGetTok(parse, temp_str, end, ",; \t\n");
	if (temp_str.empty())
	    continue;
	PIid.push_back(temp_str);
   }

/*
   fprintf(finals, "module top(");
   for (vector<string>::const_iterator iter=PIid.begin(); iter != PIid.end(); iter++)
   {
	fprintf(finals, " %s_input , ", (*iter).c_str());
   }
   fprintf(finals, "final_output);\n");
   fprintf(finals, "input ");
   for (vector<string>::const_iterator iter=PIid.begin(); iter != PIid.end()-1; iter++)
   {
	fprintf(finals, " %s_input , ", (*iter).c_str());
   }
   fprintf(finals, " %s_input ;\n", (*(PIid.end()-1)).c_str());
   fprintf(finals, "output final_output;\n");
*/
}

void
CirMgr::readPOs(size_t end)
{
   string temp_str;
   while (end != string::npos)
   {
	end = myStrGetTok(parse, temp_str, end, ",; \t\n");
	if (temp_str.empty())
	    continue;
	POid.push_back(temp_str);
   }

}


void
CirMgr::readwires_G(size_t end, FILE* goldens)
{
   string temp_str;
   fprintf(goldens, "wire ");
   end = myStrGetTok(parse, temp_str, end, ",; \t\n");
   fprintf(goldens, " %s_g1 , %s_g2 ", temp_str.c_str(), temp_str.c_str());
   while (end != string::npos)
   {
	end = myStrGetTok(parse, temp_str, end, ",; \t\n");
	if (temp_str.empty())
	    continue;
	fprintf(goldens, " , %s_g1 , %s_g2 ", temp_str.c_str(), temp_str.c_str());
   }
   fprintf(goldens, " ;\n");

}

void
CirMgr::readwires_R(size_t end, FILE* finals)
{
   string temp_str;
   fprintf(finals, "wire ");
   end = myStrGetTok(parse, temp_str, end, ",; \t\n");
   fprintf(finals, " %s_r1 , %s_r2 ", temp_str.c_str(), temp_str.c_str());
   while (end != string::npos)
   {
	end = myStrGetTok(parse, temp_str, end, ",; \t\n");
	if (temp_str.empty())
	    continue;
	fprintf(finals, " , %s_r1 , %s_r2 ", temp_str.c_str(), temp_str.c_str());
   }
   fprintf(finals, " ;\n");

}

void
CirMgr::readgates(FILE* goldens, bool ISGolden)
{
   vector<string> variables;
   string gate_type, temp_str;
   for (unsigned i=parse.size()-1; ; --i)
   {
	if (parse[i] == ')')
	{
	    parse = parse.substr(0, i);
	    break;
	}
   }

   size_t end = myStrGetTok(parse, gate_type);
   myStrGetTok(gate_type, gate_type, 0, "(");
   end = myStrGetTok(parse, temp_str, 0, "(");

   // TODO: consider port association
   bool skip_next = false;
   if (gate_type[0] == '_')
   {
	size_t dot_end = myStrGetTok(parse, temp_str, end, ".");
	if (dot_end != string::npos)  // port association occurs
	{
	    skip_next = true;
	    if (gate_type[1] == 'D')
	    {
		variables.push_back(temp_str);
		variables.push_back(temp_str);
		variables.push_back(temp_str);
		while (dot_end != string::npos)
		{
		    if (parse[dot_end+1] == 'O')
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(0) = temp_str;
		    }
		    else if (parse[dot_end+1] == 'C')
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(1) = temp_str;
		    }
		    else if (parse[dot_end+1] == 'D')
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(2) = temp_str;
		    }
		    dot_end = myStrGetTok(parse, temp_str, dot_end, ".");
		}
	    }
	    else if (gate_type[1] == 'H')
	    {
		variables.push_back(temp_str);
		variables.push_back(temp_str);
		variables.push_back(temp_str);
		variables.push_back(temp_str);
		while (dot_end != string::npos)
		{
		    if (parse[dot_end+1] == 'O')
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(0) = temp_str;
		    }
		    else if ((parse[dot_end+1] == 'I') && (parse[dot_end+2] == '0'))
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(1) = temp_str;
		    }
		    else if ((parse[dot_end+1] == 'I') && (parse[dot_end+2] == '1'))
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(2) = temp_str;
		    }
		    else if (parse[dot_end+1] == 'S')
		    {
			dot_end = myStrGetTok(parse, temp_str, dot_end, "(");
			dot_end = myStrGetTok(parse, temp_str, dot_end+1, " )\t");
			variables.at(3) = temp_str;
		    }
		    dot_end = myStrGetTok(parse, temp_str, dot_end, ".");
		}
	    }
	}
   }

   end += 1;
   if (!skip_next){
   while (end != string::npos)
   {
	end = myStrGetTok(parse, temp_str, end, ",; \t");
	if (temp_str.empty())
	    continue;
	variables.push_back(temp_str);
   }
   }

   vector<string> sec_variables;
   for (vector<string>::iterator iter=variables.begin(); iter!=variables.end(); iter++)
   {
	if ((*iter) == "1'b0")
	    sec_variables.push_back("1'b0");
	else if ((*iter) == "1'bx")
	{
	    (*iter) = "1'b0";
	    sec_variables.push_back("1'b1");
	}
	else if ((*iter) == "1'b1")
	    sec_variables.push_back("1'b1");
	else
	{
	    if (ISGolden)
	    {
		sec_variables.push_back((*iter) + "_g2");
		(*iter) += "_g1";
	    }
	    else
	    {
		sec_variables.push_back((*iter) + "_r2");
		(*iter) += "_r1";
	    }
	}
   }

   end = myStrGetTok(gate_type, gate_type);
   if (gate_type == "_DC")
	writeDCgate(variables, sec_variables, goldens);
   else if (gate_type == "_HMUX")
	writeMUXgate(variables, sec_variables, goldens);
   else if ((gate_type == "or") || (gate_type == "and") || (gate_type == "buf"))
	writeBUFgate(gate_type, variables, sec_variables, goldens);
   else if ((gate_type == "nand") || (gate_type == "nor"))
	writeNANDgate(gate_type, variables, sec_variables, goldens);
   else if ((gate_type == "xnor") || (gate_type == "xor"))
	writeXORgate(gate_type, variables, sec_variables, goldens);
   else if (gate_type == "not")
	writeNOTgate(variables, sec_variables, goldens);


}

void 
CirMgr::writeXORgate(string& gate_type, vector<string>& fir, vector<string>& sec, FILE* goldens)
{
   for (unsigned i=1, num=fir.size(); i<num; ++i)
	fprintf(goldens, "  xor(wire_%lu , %s , %s );\n", wire_num++, fir.at(i).c_str(), sec.at(i).c_str());
   fprintf(goldens, "  or(wire_%lu , ", wire_num++);
   for (unsigned i=1, num=fir.size(); i<num-1; ++i)
	fprintf(goldens, "wire_%lu , ", wire_num-i-1);
   fprintf(goldens, "wire_%lu );\n", wire_num-fir.size());

   fprintf(goldens, "  not(wire_%lu ", wire_num++);
   fprintf(goldens, ", wire_%lu );\n", wire_num - 2);


   fprintf(goldens, "  %s(wire_%lu , ", gate_type.c_str(), wire_num++);
   for (unsigned i=1, num=fir.size(); i<num-1; ++i)
	fprintf(goldens, "%s , ", fir.at(i).c_str());
   fprintf(goldens, "%s );\n", fir.at(fir.size()-1).c_str());

   //fprintf(goldens, "  %s(wire_%lu , ", gate_type.c_str(), wire_num++);
   //for (unsigned i=1, num=sec.size(); i<num-1; ++i)
	//fprintf(goldens, "%s , ", sec.at(i).c_str());
   //fprintf(goldens, "%s );\n", sec.at(sec.size()-1).c_str());

   /*if (gate_type[1] == 'n')
   {
	fprintf(goldens, "  not(wire_%lu", wire_num++);
	fprintf(goldens, ", wire_%lu);\n", wire_num - 3);
	fprintf(goldens, "  not(wire_%lu", wire_num++);
	fprintf(goldens, ", wire_%lu);\n", wire_num - 3);
	fprintf(goldens, "  and(%s, wire_%lu, wire_%lu);\n", fir.at(0).c_str(), wire_num - 5, wire_num - 2);
	fprintf(goldens, "  or(%s, wire_%lu, wire_%lu);\n", sec.at(0).c_str(), wire_num - 6, wire_num - 1);
   }*/
   //else
   //{
	//fprintf(goldens, "  and(%s , wire_%lu , wire_%lu );\n", fir.at(0).c_str(), wire_num - 3, wire_num - 2);
	//fprintf(goldens, "  or(%s , wire_%lu , wire_%lu );\n", sec.at(0).c_str(), wire_num - 4, wire_num - 1);
	fprintf(goldens, "  and(%s , wire_%lu , wire_%lu );\n", fir.at(0).c_str(), wire_num - 2, wire_num - 1);
	fprintf(goldens, "  or(%s , wire_%lu , wire_%lu );\n", sec.at(0).c_str(), wire_num - 3, wire_num - 1);
   //}

}



void 
CirMgr::writeDCgate(vector<string>& fir, vector<string>& sec, FILE* goldens)
{
   //fprintf(goldens, "  or(wire_%lu, %s, %s);\n", wire_num++, fir.at(2).c_str(), sec.at(2).c_str());
   fprintf(goldens, "  not(wire_%lu , %s );\n", wire_num++, sec.at(2).c_str());
   fprintf(goldens, "  and(%s , %s , wire_%lu );\n", fir.at(0).c_str(), fir.at(1).c_str(), wire_num - 1);
   fprintf(goldens, "  or(%s , %s , %s );\n", sec.at(0).c_str(), sec.at(1).c_str(), sec.at(2).c_str());
}

void 
CirMgr::writeMUXgate(vector<string>& fir, vector<string>& sec, FILE* goldens)
{
   fprintf(goldens, "  not(wire_%lu , %s );\n", wire_num++, fir.at(3).c_str());
   fprintf(goldens, "  not(wire_%lu , %s );\n", wire_num++, sec.at(3).c_str());
   fprintf(goldens, "  and(wire_%lu , %s ", wire_num++, fir.at(1).c_str());
   fprintf(goldens, ", wire_%lu );\n", wire_num - 2);
   fprintf(goldens, "  and(wire_%lu , %s ", wire_num++, sec.at(1).c_str());
   fprintf(goldens, ", wire_%lu );\n", wire_num - 3);

   fprintf(goldens, "  and(wire_%lu , %s , %s );\n", wire_num++, fir.at(2).c_str(), fir.at(3).c_str());
   fprintf(goldens, "  and(wire_%lu , %s , %s );\n", wire_num++, sec.at(2).c_str(), fir.at(3).c_str());
   fprintf(goldens, "  or(wire_%lu , %s , %s );\n", wire_num++, sec.at(1).c_str(), sec.at(2).c_str());

   fprintf(goldens, "  and(wire_%lu ", wire_num++);
   fprintf(goldens, ", wire_%lu , wire_%lu , %s );\n", wire_num-8, wire_num-2, sec.at(3).c_str());

   fprintf(goldens, "  and(wire_%lu , %s , %s );\n", wire_num++, fir.at(1).c_str(), fir.at(2).c_str());
   fprintf(goldens, "  or( %s , wire_%lu , wire_%lu , wire_%lu );\n", sec.at(0).c_str(), wire_num-2,wire_num-6,wire_num-4);
   fprintf(goldens, "  or( %s , wire_%lu , wire_%lu , wire_%lu );\n", fir.at(0).c_str(), wire_num-1,wire_num-7,wire_num-5);
}

void 
CirMgr::writeBUFgate(string& gate_type, vector<string>& variables, vector<string>& sec_variables, FILE* goldens)
{
   fprintf(goldens, "  %s(", gate_type.c_str());
   for (vector<string>::const_iterator iter=variables.begin(); iter!=variables.end()-1; iter++)
	fprintf(goldens, "%s , ", (*iter).c_str());
   fprintf(goldens, "%s );\n", (*(variables.end()-1)).c_str());
   fprintf(goldens, "  %s(", gate_type.c_str());
   for (vector<string>::const_iterator iter=sec_variables.begin(); iter!=sec_variables.end()-1; iter++)
	fprintf(goldens, "%s , ", (*iter).c_str());
   fprintf(goldens, "%s );\n", (*(sec_variables.end()-1)).c_str());
}

void 
CirMgr::writeNANDgate(string& gate_type, vector<string>& variables, vector<string>& sec_variables, FILE* goldens)
{
   //gate_type = gate_type.substr(1);
   fprintf(goldens, "  %s(%s , ", gate_type.c_str(), sec_variables.at(0).c_str());
   for (vector<string>::const_iterator iter=variables.begin()+1; iter!=variables.end()-1; iter++)
	fprintf(goldens, "%s , ", (*iter).c_str());
   fprintf(goldens, "%s );\n", (*(variables.end()-1)).c_str());


   fprintf(goldens, "  %s(%s , ", gate_type.c_str(), variables.at(0).c_str());
   for (vector<string>::const_iterator iter=sec_variables.begin()+1; iter!=sec_variables.end()-1; iter++)
	fprintf(goldens, "%s , ", (*iter).c_str());
   fprintf(goldens, "%s );\n", (*(sec_variables.end()-1)).c_str());
/*
   vector<string> new_first, new_second;
   string temp_str = "wire_" + to_string(wire_num-2);
   new_first.push_back(variables.at(0));
   new_first.push_back(temp_str);
   temp_str = "wire_" + to_string(wire_num-1);
   new_second.push_back(sec_variables.at(0));
   new_second.push_back(temp_str);
   writeNOTgate(new_first, new_second, goldens);
*/

}



void 
CirMgr::writeNOTgate(vector<string>& variables, vector<string>& sec_variables, FILE* goldens)
{
   string temp_str = variables.at(0), sec_temp_str = sec_variables.at(0);
   if (variables.size() > 2)
   {
	temp_str = variables.at(variables.size()-2);
	sec_temp_str = sec_variables.at(sec_variables.size()-2);
   }

   fprintf(goldens, "  not(%s , %s );\n", temp_str.c_str(), (*(sec_variables.end()-1)).c_str());
   fprintf(goldens, "  not(%s , %s );\n", sec_temp_str.c_str(), (*(variables.end()-1)).c_str());

   if (variables.size() == 2)
	return;
   for (unsigned i=0; i<variables.size()-2; ++i)
   {
	fprintf(goldens, "  buf(%s , %s );\n", variables.at(i).c_str(), temp_str.c_str());
	fprintf(goldens, "  buf(%s , %s );\n", sec_variables.at(i).c_str(), sec_temp_str.c_str());
   }
}

