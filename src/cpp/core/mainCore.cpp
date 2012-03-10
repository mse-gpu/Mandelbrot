#include <stdlib.h>
#include <iostream>

extern int launchApplication(void);
extern int launchApplicationOMP(void);

int mainCore(void);

int mainCore(void){
    //int code = launchApplication();
    int code = launchApplicationOMP();

    return code;
}
