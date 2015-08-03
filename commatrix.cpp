/*
         __       __   ______   _______   _______    ______   ________
        /  \     /  | /      \ /       \ /       \  /      \ /        |
        $$  \   /$$ |/$$$$$$  |$$$$$$$  |$$$$$$$  |/$$$$$$  |$$$$$$$$/
        $$$  \ /$$$ |$$ |  $$/ $$ |__$$ |$$ |__$$ |$$ |  $$ |$$ |__
        $$$$  /$$$$ |$$ |      $$    $$/ $$    $$< $$ |  $$ |$$    |
        $$ $$ $$/$$ |$$ |   __ $$$$$$$/  $$$$$$$  |$$ |  $$ |$$$$$/
        $$ |$$$/ $$ |$$ \__/  |$$ |      $$ |  $$ |$$ \__$$ |$$ |
        $$ | $/  $$ |$$    $$/ $$ |      $$ |  $$ |$$    $$/ $$ |
        $$/      $$/  $$$$$$/  $$/       $$/   $$/  $$$$$$/  $$/

                A Memory and Communication Profiler

 * This file is a part of MCPROF.
 * https://bitbucket.org/imranashraf/mcprof
 * 
 * Copyright (c) 2014-2015 TU Delft, The Netherlands.
 * All rights reserved.
 * 
 * MCPROF is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MCPROF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MCPROF.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Authors: Imran Ashraf
 *
 */

#include "globals.h"
#include "commatrix.h"
#include "counters.h"

extern map <string,IDNoType> FuncName2ID;
extern bool ShowUnknown;
extern u32  Threshold;

Matrix2D::Matrix2D()
{
    IDNoType cols=DEFAULT_SIZE;
    IDNoType rows=DEFAULT_SIZE;
    float value=0.0f;
    D1ECHO("Default Comm Matrix Size = " << DEFAULT_SIZE);
    Matrix.resize( cols , vector<float>( rows , value) );
}

Matrix2D::Matrix2D(IDNoType size)
{
    IDNoType cols=size;
    IDNoType rows=size;
    float value=0.0f;
    Matrix.resize( cols , vector<float>( rows , value) );
}

float Matrix2D::MaxCommunication(u16 StartID)
{
    float currmax=0.0f;
    // Following iterations can be optimized to TotalSymbols instead of size
    for (IDNoType p=StartID; p<Matrix.size(); p++)
    {
        for (IDNoType c=StartID; c<Matrix.size(); c++)
        {
            currmax = std::max(currmax, Matrix[p][c]);
        }
    }
    return currmax;
}

void Matrix2D::Print(ostream &fout)
{
    IDNoType TotalSymbols = symTable.TotalSymbolCount();
    CHECK_LT(TotalSymbols, Matrix.size());

    for (IDNoType p=0; p<TotalSymbols; p++)
    {
        for (IDNoType c=0; c<TotalSymbols; c++)
        {
            fout << setw(12) << Matrix[p][c] <<" ";
        }
        fout<<endl;
    }
}

void Matrix2D::UpdateEmptyRowsCols(IDNoType StartID, IDNoType EndID)
{
    for (IDNoType p=StartID; p<EndID; p++)
    {
        float rowsum=0.0;
        for (IDNoType c=StartID; c<EndID; c++)
        {
            float comm = Matrix[p][c];
            if ( comm > Threshold )
                rowsum+=comm;
        }
        if( rowsum == 0 )
            FilledRows.insert(p);
    }

    for (IDNoType c=StartID; c<EndID; c++)
    {
        float colsum=0.0;
        for (IDNoType p=StartID; p<EndID; p++)
        {
            float comm = Matrix[p][c];
            if ( comm > Threshold )
                colsum+=comm;
        }
        if( colsum == 0 )
            FilledCols.insert(c);
    }
}

bool Matrix2D::IsFilledRow(IDNoType r)
{
    return !(FilledRows.find(r) != FilledRows.end() );
}

bool Matrix2D::IsFilledCol(IDNoType c)
{
    return !(FilledCols.find(c) != FilledCols.end() );
}

// Use the following for properly aligned matrix print for visual inspection
// This can be problematic of width not set properly to be processed by gnuplot script
// #define ALIGNMENT (setw(25))

// Use the following to print tabs which will not be visually appealing but it will
// generate the columns properly for further processing by other tools
#define ALIGNMENT ("    ")

void Matrix2D::PrintMatrix(ostream &fout)
{
    ECHO("Printing communication matrix as table for processing by tool");
    IDNoType TotalSymbols = symTable.TotalSymbolCount();
    CHECK_LT(TotalSymbols, Matrix.size());

    u16 StartID;
    if(ShowUnknown)
        StartID = 0; //use this if you want to print unknown
    else
        StartID = 1; //use this if you dont want to print unknown

    // first update the map which contains the filled rows and columns
    UpdateEmptyRowsCols(StartID, TotalSymbols);

    fout << ALIGNMENT << " ";
    for (IDNoType c=StartID; c<TotalSymbols; c++)
    {
        if( IsFilledCol(c) )
        {
            fout << ALIGNMENT << symTable.GetSymName(c);
        }
    }
    fout << endl;

    for (IDNoType p=StartID; p<TotalSymbols; p++)
    {
        if( IsFilledRow(p) )
        {
            fout << ALIGNMENT << symTable.GetSymName(p);

            for (IDNoType c=StartID; c<TotalSymbols; c++)
            {
                if( IsFilledCol(c) )
                {
                    fout << ALIGNMENT << Matrix[p][c];
                }
            }
            fout<<endl;
        }
    }

    /******** for callstack.dot  *************/
    std::ofstream depout;
    OpenOutFile("dependencies.dat", depout);
    depout << "# producer    consumer    communication " << endl;
    for (IDNoType pid=StartID; pid<TotalSymbols; pid++)
    {
        string prod = symTable.GetSymName(pid);
        for (IDNoType cid=StartID; cid<TotalSymbols; cid++)
        {
            string cons = symTable.GetSymName(cid);
            if( Matrix[pid][cid] > 0 )
                depout << prod << " "<< cons << "  " << Matrix[pid][cid] << endl;
        }
    }
    depout.close();
    /******** for callstack.dot  *************/

}
#undef ALIGNMENT

void Matrix2D::PrintDot(ostream &dotout)
{
    ECHO("Printing communication in DOT");
    u16 TotalSymbols = GlobalID;
    D1ECHO( VAR(TotalSymbols) );
    CHECK_LT(TotalSymbols, Matrix.size());

    dotout << "digraph {\ngraph [];"
           << "\nedge [fontsize=18, arrowhead=vee, arrowsize=0.5];"
           << endl;

    string objNodeStyle(" fontcolor=black, shape=box, fontsize=22");
    string ftnNodeStyle(" fontcolor=black, style=filled, fontsize=22");

    u16 StartID;
    if(ShowUnknown)
        StartID = 0; // as 0 is reserved for unknown id
    else
        StartID = 1;

    float maxComm = MaxCommunication(StartID);

    #if (FUNCTION_ORDER == ORDERED)
    // this sorting of functions is done in the order of insertion to map
    // to print them in order in the graph
    vector<IDNoType> allFunIDs;
    map<IDNoType,string> ID2FunctionName;
    map<string,IDNoType>::iterator iter1;
    for( iter1 = FuncName2ID.begin(); iter1 != FuncName2ID.end(); iter1++)
    {
        const string& symname = iter1->first;
        IDNoType& fid = iter1->second;
        allFunIDs.push_back(fid); // insert in vector
        ID2FunctionName[fid] = symname; // make reverse map
    }

    //sort vector
    sort(allFunIDs.begin(), allFunIDs.end());

    // now print functions in order
    vector<IDNoType>::iterator iter2;
    for( iter2 = allFunIDs.begin(); iter2 != allFunIDs.end(); iter2++)
    {
        IDNoType& fid = *iter2;

        const string& symname = ID2FunctionName[fid];

        if( !symname.empty() && fid >= StartID )
        {
            dotout  << "\"" << (u16)fid << "\""
                    << " [label=\" " << symname
                    << " \\n" << GetInstrCountPercent(fid) << "%"
                    << ", " << GetCallCount(fid) << "\""
                    << ftnNodeStyle 
                    << "];" << endl;
        }
    }
    #endif

    // now print the objects
    for (u16 c=StartID; c<TotalSymbols; c++)
    {
        string symname = symTable.GetSymName(c);
        if( !symname.empty() )
        {
            if ( symTable.SymIsObj(c) )
            {
                dotout << "\"" << (u16)c << "\"" << " [label=\" " << symname
                       << " \\n" << hBytes(symTable.GetTotalSymSize(c)) << "\""
                       << objNodeStyle 
                       << "];" << endl;
            }
            #if (FUNCTION_ORDER == UNORDERED)
            else
            {
                dotout << "\"" << (u16)c << "\""
                       << " [label=\" " << symname
                       << " \\n" << GetInstrCountPercent(c) << "%"
                       << ", " << GetCallCount(c) << "\""
                       << ftnNodeStyle 
                       << "];" << endl;
            }
            #endif
        }
    }

    int color;
    // now print the edges
    for (u16 p=StartID; p<TotalSymbols; p++)
    {
        for (u16 c=StartID; c<TotalSymbols; c++)
        {
            float comm = Matrix[p][c];
            if( comm > Threshold )
            {
                color = (int) (  1023 *  log((float)(comm)) / log((float)maxComm)  );
                dotout << dec
                       << "\"" << (u16)p << "\""
                       << "->"
                       << "\"" << (u16)c << "\""
                       << "[ label=\""
                       << hBytes(comm) <<"\""
                       << " color = \"#"
                       << hex
                       << setw(2) << setfill('0') << max(0, color-768)
                       << setw(2) << setfill('0') << min(255, 512-(int)abs(color-512))
                       << setw(2) << setfill('0') << max(0, min(255,512-color))
                       << "\""
                       << "]"
                       << endl;
            }
        }
    }

    dotout << "}" << endl;
}
