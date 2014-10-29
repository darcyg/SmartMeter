/* Generate a sine table suitable for use with a DDS
   routine. */

#include <math.h>
#include <stdio.h>

#define TABLE_SIZE      128

int main (int argc, char *argv[])
{
    int i;
    float f;

    printf("{");
    for (i = 0;  i <= TABLE_SIZE - 1;  i++)
    {
        if ((i % 8) == 0)
            printf("\n    ");
        f = ((i + 0.5)/128.0)*(M_PI/2.0);
        f = sin(f);
        printf("0x%04X", (int) (32768.0*f));
        if (i < TABLE_SIZE - 1)
            printf(",");
    }
    printf("\n};\n");
    return  0;
}
