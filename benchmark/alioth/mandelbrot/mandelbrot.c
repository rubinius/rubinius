/*The Computer Language Benchmarks Game
  http://shootout.alioth.debian.org/

  contributed by Paolo Bonzini
*/


#include <stdio.h>
#include <stdlib.h>

typedef double v2df __attribute__ ((vector_size(16))); // vector of two double floats
typedef int v4si __attribute__ ((vector_size(16))); // vector of four 32-bit ints

int main (int argc, char **argv)
{
    int w, h, bit_num = 0;
    char byte_acc = 0;
    int i, iter = 50;
    double x, y;
    v2df zero = { 0.0, 0.0 };
    v2df four = { 4.0, 4.0 };
    v2df nzero = -zero;

    /* Detect endianness.  */
    v4si mask = (v4si)nzero;
    int *pmask = (int *) &mask;
    if (pmask[1]) pmask++;

    w = h = atoi(argv[1]);

    printf("P4\n%d %d\n",w,h);

    for(y=0;y<h;++y)
    {
        for(x=0;x<w;x+=2)
        {
	   v2df Crv = { 2*x/w-1.5, 2*(x+1.0)/w-1.5 };
	   v2df Civ = { 2*y/h-1.0, 2*y/h-1.0 };
	   v2df Zrv = { 0.0, 0.0 };
	   v2df Ziv = { 0.0, 0.0 };
            v2df Trv = { 0.0, 0.0 };
	   v2df Tiv = { 0.0, 0.0 };

            i = 0;
            do {
                Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
                Zrv = Trv - Tiv + Crv;
                Trv = Zrv * Zrv;
                Tiv = Ziv * Ziv;

		/* sign bit zeroed if 4.0 - Trv - Tiv >= 0.0 (i.e. |Z| <= 4.0).  */
		v2df delta = four - Trv - Tiv;
		mask = (v4si)delta & (v4si)nzero;
            } while (++i < iter && !(pmask[0] && pmask[2]));

            byte_acc <<= 2;
            if(!pmask[0])
                byte_acc |= 0x02;
            if(!pmask[2])
                byte_acc |= 0x01;
            bit_num+=2;

            if(bit_num == 8) {
                fputc(byte_acc,stdout);
                byte_acc = 0;
                bit_num = 0;
            }
        }

        if(bit_num != 0) {
            byte_acc <<= (8-w%8);
            fputc(byte_acc,stdout);
            byte_acc = 0;
            bit_num = 0;
        }
    }
}

