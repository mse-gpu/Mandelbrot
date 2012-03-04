#include <iostream>
#include <cmath> //for sin
#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"
#include "CalibreurCudas.h"

class RipplingFunctionalImage : public ImageFonctionelSelectionMOOs {
    public:
	RipplingFunctionalImage(int m, int n, DomaineMaths domain);

	void setT(float t);

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float color(float x, float y);
	float d(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);

	float t;
};

RipplingFunctionalImage::RipplingFunctionalImage(int m, int n, DomaineMaths domain) : ImageFonctionelSelectionMOOs(m,n,domain), t(1) {
    onDomaineChangePerformed(domain);
}

void RipplingFunctionalImage::setT(float newT){
    t = newT;

    refreshAll(getCurrentDomaine());
}

void RipplingFunctionalImage::refreshAll(const DomaineMaths& domainNew){
    int w = getW();
    int h = getH();

    float dx = (float) (domainNew.dx / (float) w);
    float dy = (float) (domainNew.dy / (float) h);
    float y = domainNew.y0;

    for(int i = 1; i <= h; ++i){
	float x = domainNew.x0;

	for(int j = 1; j <= w; ++j){
	    float c = color(x,y);

	    setRGBA(i, j, c, c, c);

	    x += dx;
	}

	y += dy;
    }
}

void RipplingFunctionalImage::onDomaineChangePerformed(const DomaineMaths& domainNew){
    refreshAll(domainNew);
}

float RipplingFunctionalImage::color(float x, float y){
    return 128 + 127 * ((cos(d(x,y) / (float)10 -(t / (float)7))) / (d(x,y) / 10 + 1));
}

float RipplingFunctionalImage::d(float x, float y){
    float fx = x - (getW() / 2);
    float fy = y - (getH() / 2);

    return sqrt(fx * fx + fy * fy);
}

class RipplingGLImage : public GLImageFonctionelSelections {
    public:
	RipplingGLImage(RipplingFunctionalImage *ptrImageFonctionel);

    private:
	float t;
	float dt;

	void idleFunc();

	RipplingFunctionalImage* image;
};

RipplingGLImage::RipplingGLImage(RipplingFunctionalImage *ptrImageFonctionel) : GLImageFonctionelSelections(ptrImageFonctionel), image(ptrImageFonctionel) {
    t = 1;

    //Define the speed of the animation
    dt = 2 * (atan(1) * 4) / (float) 36;
}

void RipplingGLImage::idleFunc(){
    t += dt;
    image->setT(t);
    updateView();
}

extern int launchApplication(){
    std::cout << "Launch the application" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    int dim = 600;

    DomaineMaths domain(0, 0, dim, dim);
   
    RipplingFunctionalImage* functionalImage = new RipplingFunctionalImage(dim, dim, domain);
    RipplingGLImage* functionSelections = new RipplingGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
