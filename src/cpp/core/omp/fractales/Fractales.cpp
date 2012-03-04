#include <iostream>
#include <cmath> //for sin
#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"
#include "CalibreurCudas.h"

class MandelBrotFunctionalImage : public ImageFonctionelSelectionMOOs {
    public:
	MandelBrotFunctionalImage(int m, int n, DomaineMaths domain);

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float color(float x, float y);
	float d(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);
};

MandelBrotFunctionalImage::MandelBrotFunctionalImage(int m, int n, DomaineMaths domain) : ImageFonctionelSelectionMOOs(m,n,domain) {
    onDomaineChangePerformed(domain);
}

void MandelBrotFunctionalImage::refreshAll(const DomaineMaths& domainNew){
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

void MandelBrotFunctionalImage::onDomaineChangePerformed(const DomaineMaths& domainNew){
    refreshAll(domainNew);
}

float MandelBrotFunctionalImage::color(float x, float y){
    return 128 + 127 * ((cos(d(x,y) / (float)10 -(10 / (float)7))) / (d(x,y) / 10 + 1));
}

float MandelBrotFunctionalImage::d(float x, float y){
    float fx = x - (getW() / 2);
    float fy = y - (getH() / 2);

    return sqrt(fx * fx + fy * fy);
}

extern int launchApplication(){
    std::cout << "Launch the application" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    int dim = 600;

    DomaineMaths domain(0, 0, dim, dim);
   
    MandelBrotFunctionalImage* functionalImage = new MandelBrotFunctionalImage(dim, dim, domain);
    GLImageFonctionelSelections* functionSelections = new GLImageFonctionelSelections(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
