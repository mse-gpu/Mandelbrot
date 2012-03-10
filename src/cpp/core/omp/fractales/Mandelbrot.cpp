#include <iostream>
#include <cmath>

#include "omp.h"

#include "DomaineMaths.h"
#include "GLUTWindowManagers.h"
#include "ImageFonctionelSelectionMOOs.h"
#include "GLImageFonctionelSelections.h"

class MandelBrotFunctionalImage : public ImageFonctionelSelectionMOOs {
    public:
	MandelBrotFunctionalImage(int m, int n, DomaineMaths domain);

	void setN(int n);

    protected:
	void onDomaineChangePerformed(const DomaineMaths& domainNew);

    private:
	float mandelbrot(float x, float y);

	void refreshAll(const DomaineMaths& domainNew);

	int N;
};

MandelBrotFunctionalImage::MandelBrotFunctionalImage(int m, int n, DomaineMaths domain) : ImageFonctionelSelectionMOOs(m,n,domain), N(10) {
    //Init the domain
    onDomaineChangePerformed(domain);
}

void MandelBrotFunctionalImage::setN(int n){
    N = n;

    refreshAll(getCurrentDomaine());
}

void MandelBrotFunctionalImage::onDomaineChangePerformed(const DomaineMaths& domainNew){
    //Repaint everything
    refreshAll(domainNew);
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
	    float h = mandelbrot(x, y);

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

float MandelBrotFunctionalImage::mandelbrot(float x, float y){
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

class MandelBrotGLImage : public GLImageFonctionelSelections {
    public:
	MandelBrotGLImage(MandelBrotFunctionalImage* image);

    private:
	float N;
	float acc;

	void idleFunc();

	MandelBrotFunctionalImage* image;
};

MandelBrotGLImage::MandelBrotGLImage(MandelBrotFunctionalImage* newImage) : GLImageFonctionelSelections(newImage), image(newImage) {
    N = 10;
    acc = 0;
}

void MandelBrotGLImage::idleFunc(){
    ++acc;

    //Do not change everytime to make something smoother
    if(acc == 1000){
	++N;
	image->setN(N);
	updateView();
	acc = 0;
    }
}

extern int launchMandelbrot(){
    std::cout << "Launch the application" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    float xMin = -1.3968;
    float xMax = -1.3578;
    float yMin = -0.03362;
    float yMax = 0.0013973;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    int w = 800;
    int h = 600;
   
    MandelBrotFunctionalImage* functionalImage = new MandelBrotFunctionalImage(w, h, domain);
    MandelBrotGLImage* functionSelections = new MandelBrotGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}
