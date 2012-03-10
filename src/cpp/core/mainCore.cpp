#include <stdlib.h>
#include <iostream>

extern int launchJulia(void);
extern int launchMandelbrot(void);
extern int launchMandelbrotOMP(void);

int mainCore(void);

int mainCore(void){
    //int code = launchMandelbrot();
    int code = launchMandelbrotOMP();
    //int code = launchJulia();

    return code;
}
