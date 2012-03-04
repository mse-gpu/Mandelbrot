#include <iostream>
#include <cmath> //for sin
#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"
#include "CalibreurCudas.h"

#define THREADS 12

class RipplingFunctionalImageOMP : public ImageFonctionelSelectionMOOs {
    public:
	RipplingFunctionalImageOMP(int m, int n, DomaineMaths domain);

	void setT(float t);

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float color(float x, float y);
	float d(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);

	float t;
};

RipplingFunctionalImageOMP::RipplingFunctionalImageOMP(int m, int n, DomaineMaths domain) : ImageFonctionelSelectionMOOs(m,n,domain), t(1) {
    onDomaineChangePerformed(domain);
}

void RipplingFunctionalImageOMP::setT(float newT){
    t = newT;

    refreshAll(getCurrentDomaine());
}

void RipplingFunctionalImageOMP::refreshAll(const DomaineMaths& domainNew){
    const int w = getW();
    const int h = getH();

    const float dx = (float) (domainNew.dx / (float) w);
    const float dy = (float) (domainNew.dy / (float) h);

    #pragma omp parallel
    {
	int tid = omp_get_thread_num();
	int i = tid + 1;

	float y = domainNew.y0 + tid * dy;

	while(i <= h){
	    float x = domainNew.x0;

	    for(int j = 1; j <= w; ++j){
		float c = color(x,y);

		setRGBA(i, j, c, c, c, 255);

		x += dx;
	    }

	    y += THREADS * dy;

	    i += THREADS;
	}
    }
}

void RipplingFunctionalImageOMP::onDomaineChangePerformed(const DomaineMaths& domainNew){
    refreshAll(domainNew);
}

float RipplingFunctionalImageOMP::color(float x, float y){
    return 128 + 127 * ((cos(d(x,y) / (float)10 -(t / (float)7))) / (d(x,y) / 10 + 1));
}

float RipplingFunctionalImageOMP::d(float x, float y){
    float fx = x - (getW() / 2);
    float fy = y - (getH() / 2);

    return sqrt(fx * fx + fy * fy);
}

class RipplingGLImageOMP : public GLImageFonctionelSelections {
    public:
	RipplingGLImageOMP(RipplingFunctionalImageOMP *ptrImageFonctionel);

    private:
	float t;
	float dt;

	void idleFunc();

	RipplingFunctionalImageOMP* image;
};

RipplingGLImageOMP::RipplingGLImageOMP(RipplingFunctionalImageOMP* ptrImageFonctionel) : GLImageFonctionelSelections(ptrImageFonctionel), image(ptrImageFonctionel) {
    t = 1;

    //Define the speed of the animation
    dt = 2 * (atan(1) * 4) / (float) 36;
}

void RipplingGLImageOMP::idleFunc(){
    t += dt;
    image->setT(t);
    updateView();
}

extern int launchApplicationOMP(){
    omp_set_num_threads(THREADS);

    std::cout << "Launch the application with OMP" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    int dim = 600;

    DomaineMaths domain(0, 0, dim, dim);
   
    RipplingFunctionalImageOMP* functionalImage = new RipplingFunctionalImageOMP(dim, dim, domain);
    RipplingGLImageOMP* functionSelections = new RipplingGLImageOMP(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
