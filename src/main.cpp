#include <iostream>
#include <limits.h>

extern int mainCore(void);
extern int mainTest(void);

int main(void)
{
    std::cout << "Launch the tests" << std::endl;
    int code = mainTest();

    std::cout << "Launch the graphical interface" << std::endl;
    mainCore();

    return code;
}
