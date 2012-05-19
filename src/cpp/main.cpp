#include <iostream>
#include <limits.h>
#include <cmath>

#include "omp.h"
#include "ChronoOMPs.h"

#include "Fractales.hpp"
#include "JuliaSequential.hpp"
#include "JuliaOMP.hpp"
#include "MandelBrotSequential.hpp"
#include "MandelBrotOMP.hpp"

//The MandelBrot Fractales Launchers
int launchMandelbrot();
int launchMandelbrotOMP();

//The Julia Fractales Launchers
int launchJulia();
int launchJuliaOMP();

//The benchmark
int bench();

int main(void){
    //return launchMandelbrot();
    //return launchMandelbrotOMP();
    //return launchJulia();
    //return launchJuliaOMP();
    return bench();
}

int launchMandelbrot(){
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

    MandelBrotImageSequential* functionalImage = new MandelBrotImageSequential(w, h, domain);
    FractaleGLImage* functionSelections = new FractaleGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}

int launchMandelbrotOMP(){
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

    MandelBrotImageOMP* functionalImage = new MandelBrotImageOMP(w, h, domain);
    FractaleGLImage* functionSelections = new FractaleGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}

int launchJulia(){
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

    JuliaImageSequential* functionalImage = new JuliaImageSequential(w, h, domain, -0.745, +0.1);
    FractaleGLImage* functionSelections = new FractaleGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}

int launchJuliaOMP(){
    omp_set_num_threads(THREADS);

    std::cout << "Launch Julia with OMP" << std::endl;

    char** argv = NULL;
    GLUTWindowManagers::init(0, argv);

    float xMin = -1.7;
    float xMax = +1.7;
    float yMin = -1.1;
    float yMax = +1.1;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    int w = 800;
    int h = 600;

    JuliaImageOMP* functionalImage = new JuliaImageOMP(w, h, domain, -0.745, +0.1);
    FractaleGLImage* functionSelections = new FractaleGLImage(functionalImage);

    GLUTWindowManagers* windowManager = GLUTWindowManagers::getInstance();
    windowManager->createWindow(functionSelections);
    windowManager->runALL(); //This call is blocking

    return 0;
}

#define DIM_H 2000
#define DIM_W 2000
#define TIMES 20
#define THREADS 12

struct rgba {
	int r;
	int g;
	int b;
	int a;
};

void setFloatRGBA(rgba* image, int i, int j, int r, int g, int b, int a){
    image[i * (DIM_H) + j].r = r;
    image[i * (DIM_H) + j].g = g;
    image[i * (DIM_H) + j].b = b;
    image[i * (DIM_H) + j].a = a;
}

float mandelbrot(float x, float y, int N){
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

void benchSequentialMandelBrot(rgba* image, int N){
    float xMin = -1.7;
    float xMax = +1.7;
    float yMin = -1.1;
    float yMax = +1.1;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    float dx = (float) (domain.dx / (float) DIM_W);
    float dy = (float) (domain.dy / (float) DIM_H);
    float y = domain.y0;

    for(int i = 1; i <= DIM_H; ++i){
	float x = domain.x0;

	for(int j = 1; j <= DIM_W; ++j){
	    float h = mandelbrot(x, y, N);

	    setFloatRGBA(image, i, j, h, h, h, 255);

	    x += dx;
	}

	y += dy;
    }
}

void benchParallelMandelBrot(rgba* image, int N){
    omp_set_num_threads(THREADS);

    float xMin = -1.7;
    float xMax = +1.7;
    float yMin = -1.1;
    float yMax = +1.1;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);
    float dx = (float) (domain.dx / (float) DIM_W);
    float dy = (float) (domain.dy / (float) DIM_H);

    #pragma omp parallel
    {
    	int tid = omp_get_thread_num();
    	int i = tid + 1;

    	float y = domain.y0 + tid * dy;

    	while(i <= DIM_H){
    	    float x = domain.x0;

    	    for(int j = 1; j <= DIM_W; ++j){
    		float h = mandelbrot(x, y, N);

		setFloatRGBA(image, i, j, h, h, h, 255);

    		x += dx;
    	    }

    	    y += THREADS * dy;

    	    i += THREADS;
    	}
    }
}

static float cReal = -0.745;
static float cImag = +0.1;

float julia(float x, float y, int N){
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

void benchParallelJulia(rgba* image, int N){
    omp_set_num_threads(THREADS);

    float xMin = -1.7;
    float xMax = +1.7;
    float yMin = -1.1;
    float yMax = +1.1;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    float dx = (float) (domain.dx / (float) DIM_W);
    float dy = (float) (domain.dy / (float) DIM_H);

    #pragma omp parallel
    {
    	int tid = omp_get_thread_num();
    	int i = tid + 1;

    	float y = domain.y0 + tid * dy;

    	while(i <= DIM_H){
    	    float x = domain.x0;

    	    for(int j = 1; j <= DIM_W; ++j){
    		float h = julia(x, y, N);

    		setFloatRGBA(image, i, j, h, h, h, 255);

    		x += dx;
    	    }

    	    y += THREADS * dy;

    	    i += THREADS;
    	}
    }
}

void benchSequentialJulia(rgba* image, int N){
    float xMin = -1.7;
    float xMax = +1.7;
    float yMin = -1.1;
    float yMax = +1.1;

    DomaineMaths domain(xMin, yMin, xMax - xMin, yMax - yMin);

    float dx = (float) (domain.dx / (float) DIM_W);
    float dy = (float) (domain.dy / (float) DIM_H);
    float y = domain.y0;

    for(int i = 1; i <= DIM_H; ++i){
    	float x = domain.x0;

    	for(int j = 1; j <= DIM_W; ++j){
    	    float h = julia(x, y, N);

    	    setFloatRGBA(image, i, j, h, h, h, 255);

    	    x += dx;
    	}

    	y += dy;
    }
}

void benchJulia(rgba* image, int N){
    std::cout << "Launch the Julia benchmark with N=" << N << std::endl;

    ChronoOMPs chronos;
    chronos.start();

    for(int i = 0; i < TIMES; ++i){
	benchSequentialJulia(image, N);
    }

    double timeSequential = chronos.timeElapse();
    std::cout << "Sequential Total (" << TIMES << " times) " << timeSequential << "s" << std::endl;
    std::cout << "Sequential Mean  (" << TIMES << " times) " << (timeSequential / TIMES) << "s" << std::endl;

    chronos.start();

    for(int i = 0; i < TIMES; ++i){
	benchParallelJulia(image, N);
    }

    double timeParallel = chronos.timeElapse();
    std::cout << "OMP Total (" << TIMES << " times) " << timeParallel << "s" << std::endl;
    std::cout << "OMP Mean  (" << TIMES << " times) " << (timeParallel / TIMES) << "s" << std::endl;

    std::cout << "Factor=" << (timeSequential / timeParallel) << std::endl;
}

void benchMandelbrot(rgba* image, int N){
    std::cout << "Launch the MandelBrot benchmark with N=" << N << std::endl;

    ChronoOMPs chronos;
    chronos.start();

    for(int i = 0; i < TIMES; ++i){
	benchSequentialMandelBrot(image, N);
    }

    double timeSequential = chronos.timeElapse();
    std::cout << "Sequential Total (" << TIMES << " times) " << timeSequential << "s" << std::endl;
    std::cout << "Sequential Mean  (" << TIMES << " times) " << (timeSequential / TIMES) << "s" << std::endl;

    chronos.start();

    for(int i = 0; i < TIMES; ++i){
	benchParallelMandelBrot(image, N);
    }

    double timeParallel = chronos.timeElapse();
    std::cout << "OMP Total (" << TIMES << " times) " << timeParallel << "s" << std::endl;
    std::cout << "OMP Mean  (" << TIMES << " times) " << (timeParallel / TIMES) << "s" << std::endl;

    std::cout << "Factor=" << (timeSequential / timeParallel) << std::endl;
}

int bench(){
    std::cout << "Launch the Fractales benchmark" << std::endl;

    rgba* image = (rgba*) calloc(sizeof(rgba),  (DIM_H + 1) * (DIM_W + 1));

    benchMandelbrot(image, 10);
    benchJulia(image, 10);

    benchMandelbrot(image, 25);
    benchJulia(image, 25);

    benchMandelbrot(image, 50);
    benchJulia(image, 50);

    benchMandelbrot(image, 100);
    benchJulia(image, 100);

    free(image);

    return 0;
}
