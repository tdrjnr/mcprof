#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
typedef int TYPE;

unsigned int *srcArr1;
unsigned int *srcArr2;
unsigned int *sqrArr;
unsigned int *sumArr;
unsigned int *diffArr;

unsigned int coeff = 2;

void initVecs()
{
    int i;
    for(i = 0; i < SIZE; i++) {
        srcArr1[i]=i*5 + 7;
        srcArr2[i]=2*i - 3;
    }
}

void sumVecs()
{
    int i;
    for(i = 0; i < SIZE; i++)
    {
        sumArr[i] = srcArr1[i] + coeff * srcArr2[i];
    }
}

void diffVecs()
{
    int i;
    for(i = 0; i < SIZE; i++)
    {
        diffArr[i] = coeff * (srcArr1[i] - srcArr2[i]);
    }
}

TYPE SquareRootRounded(TYPE a_nInput)
{
    TYPE op  = a_nInput;
    TYPE res = 0;
    TYPE one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for TYPE type

    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
        one >>= 2;

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }

    /* Do arithmetic rounding to nearest integer */
    if (op > res)
    {
        res++;
    }

    return res;
}

void sqrootVecs()
{
    int i;
    for(i = 0; i < SIZE; i++)
    {
        sqrArr[i] = SquareRootRounded( sumArr[i] );
        sqrArr[i] = SquareRootRounded( sqrArr[i] + diffArr[i] );
    }
}

int main()
{
    printf("Vector Operations Test.\n");

    ////////////////////////////////////////////////////////////////////
    // These are dummy calculations used to make first access to memory
    // allocation functions so that there symbols are resolved for later
    // accesses, Do we need it with new implementation? TEST!
    char *dummy = malloc(1);
    dummy = calloc(1, sizeof(char) );
    dummy = realloc(dummy, 2);
    printf("Dummy malloc addr : %p\n", (void *)dummy );
    free(dummy);
    ////////////////////////////////////////////////////////////////////

    srcArr1 = calloc(SIZE/2 * sizeof(TYPE), sizeof(TYPE) );
    printf("srcArr1 addr after calloc : %p\n",srcArr1);

    srcArr2 = malloc(SIZE * sizeof(TYPE));
    printf("srcArr2 addr : %p\n",srcArr2);

    srcArr1 = realloc(srcArr1, SIZE * sizeof(TYPE));
    printf("srcArr1 addr after realloc: %p\n",srcArr1);

    sumArr = malloc(SIZE * sizeof(TYPE));
    printf("sumArr addr : %p\n",sumArr);

    diffArr = malloc(SIZE * sizeof(TYPE));
    printf("diffArr addr : %p\n",diffArr);

    sqrArr = malloc(SIZE * sizeof(TYPE));
    printf("sqrArr addr : %p\n",sqrArr);

    initVecs();
    sumVecs();
    diffVecs();
    sqrootVecs();

    printf("output : %d\n",sumArr[1]+diffArr[1]+sqrArr[1]);

    free(srcArr1);
    free(srcArr2);
    free(sumArr);
    free(diffArr);
    free(sqrArr);

    return 0;
}
