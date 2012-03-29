#include <iostream>
#include <limits.h>

//The MandelBrot Fractales Launchers
extern int launchMandelbrot(void);
extern int launchMandelbrotOMP(void);

//The Julia Fractales Launchers
extern int launchJulia(void);
extern int launchJuliaOMP(void);

int main(void){
    int code = launchMandelbrot();
    //int code = launchMandelbrotOMP();
    //int code = launchJulia();
    //int code = launchJuliaOMP();

    return code;
}
