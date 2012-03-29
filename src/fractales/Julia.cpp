#include <iostream>
#include <cmath>

#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"

class JuliaFunctionalImage : public ImageFonctionelSelectionMOOs {
    public:
	JuliaFunctionalImage(int m, int n, DomaineMaths domain, float cReal, float cImag);

	void incN();

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float julia(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);

	const float cReal;
	const float cImag;

	int N;
};

JuliaFunctionalImage::JuliaFunctionalImage(int m, int n, DomaineMaths domain, float cReal, float cImag) : ImageFonctionelSelectionMOOs(m,n,domain), cReal(cReal), cImag(cImag), N(10) {
    //Init the domain
    onDomaineChangePerformed(domain);
}

void JuliaFunctionalImage::incN(){
    ++N;

    refreshAll(getCurrentDomaine());
}

void JuliaFunctionalImage::onDomaineChangePerformed(const DomaineMaths& domainNew){
    N = 10;

    //Repaint everything
    refreshAll(domainNew);
}

void JuliaFunctionalImage::refreshAll(const DomaineMaths& domainNew){
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

float JuliaFunctionalImage::julia(float x, float y){
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

class JuliaGLImage : public GLImageFonctionelSelections {
    public:
	JuliaGLImage(JuliaFunctionalImage* image);

    private:
	float acc;

	void idleFunc();

	JuliaFunctionalImage* image;
};

JuliaGLImage::JuliaGLImage(JuliaFunctionalImage* newImage) : GLImageFonctionelSelections(newImage), image(newImage) {
    acc = 0;
}

void JuliaGLImage::idleFunc(){
    ++acc;

    //Do not change everytime to make something smoother
    if(acc == 1000){
	image->incN();
	updateView();
	acc = 0;
    }
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
   
    JuliaFunctionalImage* functionalImage = new JuliaFunctionalImage(w, h, domain, -0.745, +0.1);
    JuliaGLImage* functionSelections = new JuliaGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
