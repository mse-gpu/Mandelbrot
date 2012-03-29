#include <iostream>
#include <cmath>

#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"

#define THREADS 12

class MandelBrotFunctionalImageOMP : public ImageFonctionelSelectionMOOs {
    public:
	MandelBrotFunctionalImageOMP(int m, int n, DomaineMaths domain);

	void incN();

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float mandelbrot(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);

	int N;
};

MandelBrotFunctionalImageOMP::MandelBrotFunctionalImageOMP(int m, int n, DomaineMaths domain) : ImageFonctionelSelectionMOOs(m,n,domain), N(10) {
    //Init the domain
    onDomaineChangePerformed(domain);
}

void MandelBrotFunctionalImageOMP::incN(){
    ++N;

    refreshAll(getCurrentDomaine());
}

void MandelBrotFunctionalImageOMP::onDomaineChangePerformed(const DomaineMaths& domainNew){
    //Repaint everything
    refreshAll(domainNew);
}

void MandelBrotFunctionalImageOMP::refreshAll(const DomaineMaths& domainNew){
    int w = getW();
    int h = getH();

    float dx = (float) (domainNew.dx / (float) w);
    float dy = (float) (domainNew.dy / (float) h);

    #pragma omp parallel
    {
	int tid = omp_get_thread_num();
	int i = tid + 1;

	float y = domainNew.y0 + tid * dy;

	while(i <= h){
	    float x = domainNew.x0;

	    for(int j = 1; j <= w; ++j){
		float h = mandelbrot(x, y);

		//setFloatRGBA(i, j, h, h, h);
		if(h == 0){
		    setHSB(i, j, 0, 0, 0);
		} else {
		    setHSB(i, j, h, 1.0, 1.0);
		}

		x += dx;
	    }

	    y += THREADS * dy;

	    i += THREADS;
	}
    }
}

float MandelBrotFunctionalImageOMP::mandelbrot(float x, float y){
    float imag = 0.0;
    float real = 0.0;

    float n = 0;
    float norm;

    do{
	float tmpReal = real;
	real = real * real - imag * imag + x;
	imag = tmpReal * imag + imag * tmpReal + y;

	++n;

	norm = sqrt(real * real + imag * imag);
    } while (norm <= 2.0 && n < N);

    return n == N ? 0 : (n / (float) N);
}

class MandelBrotGLImageOMP : public GLImageFonctionelSelections {
    public:
	MandelBrotGLImageOMP(MandelBrotFunctionalImageOMP* image);

    private:
	float acc;

	void idleFunc();

	MandelBrotFunctionalImageOMP* image;
};

MandelBrotGLImageOMP::MandelBrotGLImageOMP(MandelBrotFunctionalImageOMP* newImage) : GLImageFonctionelSelections(newImage), image(newImage) {
    acc = 0;
}

void MandelBrotGLImageOMP::idleFunc(){
    ++acc;

    //Do not change everytime to make something smoother
    if(acc == 10000){
	image->incN();
	updateView();
	acc = 0;
    }
}

extern int launchMandelbrotOMP(){
    omp_set_num_threads(THREADS);

    std::cout << "Launch the application with OMP" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    float xMin = -1.3968;
    float xMax = -1.3578;
    float yMin = -0.03362;
    float yMax = 0.0013973;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    int w = 800;
    int h = 600;

    MandelBrotFunctionalImageOMP* functionalImage = new MandelBrotFunctionalImageOMP(w, h, domain);
    MandelBrotGLImageOMP* functionSelections = new MandelBrotGLImageOMP(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
