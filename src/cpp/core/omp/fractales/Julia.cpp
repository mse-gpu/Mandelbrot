#include <iostream>
#include <cmath>

#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"

template<int N>
class JuliaFunctionalImage : public ImageFonctionelSelectionMOOs {
    public:
	JuliaFunctionalImage(int m, int n, DomaineMaths domain, float cReal, float cImag);

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float julia(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);

	const float cReal;
	const float cImag;
};

template<int N>
JuliaFunctionalImage<N>::JuliaFunctionalImage(int m, int n, DomaineMaths domain, float cReal, float cImag) : ImageFonctionelSelectionMOOs(m,n,domain), cReal(cReal), cImag(cImag) {
    //Init the domain
    onDomaineChangePerformed(domain);
}

template<int N>
void JuliaFunctionalImage<N>::onDomaineChangePerformed(const DomaineMaths& domainNew){
    //Repaint everything
    refreshAll(domainNew);
}

template<int N>
void JuliaFunctionalImage<N>::refreshAll(const DomaineMaths& domainNew){
    int w = getW();
    int h = getH();

    float dx = (float) (domainNew.dx / (float) w);
    float dy = (float) (domainNew.dy / (float) h);
    float y = domainNew.y0;

    for(int i = 1; i <= h; ++i){
	float x = domainNew.x0;

	for(int j = 1; j <= w; ++j){
	    float h = julia(x, y);

	    //setFloatRGBA(i, j, h, h, h);
	    if(h == 0){
		setHSB(i, j, 0, 0, 0);
	    } else {
		setHSB(i, j, h, 1.0, 1.0);
	    }

	    x += dx;
	}

	y += dy;
    }
}

template<int N>
float JuliaFunctionalImage<N>::julia(float x, float y){
    float real = x;
    float imag = y;

    float n = 0;
    float norm;

    do{
	float tmpReal = real;
	real = real * real - imag * imag + cReal;
	imag = tmpReal * imag + imag * tmpReal + cImag;

	++n;

	norm = sqrt(real * real + imag * imag);
    } while (norm <= 2.0 && n < N);

    return n == N ? 0 : (n / (float) N);
}

extern int launchJulia(){
    std::cout << "Launch Julia" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    float xMin = -1.7;
    float xMax = +1.7;
    float yMin = -1.1;
    float yMax = +1.1;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    int w = 800;
    int h = 600;
   
    JuliaFunctionalImage<300>* functionalImage = new JuliaFunctionalImage<300>(w, h, domain, -0.745, +0.1);
    GLImageFonctionelSelections* functionSelections = new GLImageFonctionelSelections(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
