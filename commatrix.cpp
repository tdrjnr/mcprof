#include "globals.h"
#include "commatrix.h"

Matrix2D ComMatrix;

void RecordCommunication(FtnNo prod, FtnNo cons, int size)
{
    ComMatrix.Increment(prod, cons, size);
}

void PrintCommunication(ostream &fout, u16 TotalFtns)
{
    ComMatrix.Print(fout, TotalFtns);
}

void PrintMatrix(ostream &fout, u16 TotalFtns)
{
    ComMatrix.PrintMatrix(fout, TotalFtns);
}

void PrintCommunicationDot(ostream &dotout, u16 TotalFtns)
{
    ComMatrix.PrintDot(dotout, TotalFtns);
}
