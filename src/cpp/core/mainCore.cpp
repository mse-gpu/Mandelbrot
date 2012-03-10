#include <stdlib.h>
#include <iostream>

extern int launchMandelbrot(void);
extern int launchMandelbrotOMP(void);

int mainCore(void);

int mainCore(void){
    //int code = launchMandelbrot();
    int code = launchMandelbrotOMP();

    return code;
}
