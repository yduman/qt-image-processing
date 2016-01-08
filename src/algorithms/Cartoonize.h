#ifndef CARTOONIZE_H
#define CARTOONIZE_H

#include "Algorithm.h"

class Cartoonize : public Algorithm
{
public:
    void process(const Parameters &params, const Image &src, Image &dst);

    double** createMatrix(int height, int width);

    // Bilateral Filter
    Image bilFilter(const Image &img);
    Pixel bilKernel(const Image &img, int y, int x);
    bool isInRange(int size, int y, int x, const int height, const int width);
    double calcKernelField(const Image &img, int y, int x, int k, int l);
    double calcDomain(int y, int x, int k, int l);
    double calcEdge(const Image &img, int y, int x, int k, int l);
    double** initKernel(const Image &img, int y, int x);

    // Binary Filter
    Image binFilter(const Image &img, char op);
    Image binTraverse(const Image &img);
    Image edgeStrength(Image &imgX, Image &imgY);
    Image edgeDetection(Image &img);
    Pixel binKernel(const Image &img, int y, int x, char op);
    Pixel calcEdgeStrength(int y, int x, Image &imgX, Image &imgY);
    Pixel calcEdgeDetection(int y, int x, Image &img);
    double euklid(int r, int g, int b);
    double adjustPixelCol(double pixel);
    double** getBlurKernel();
    double** getSobelKernel(char c);

    // Cartoonize Filter
    Image cartFilter(Image &bilateral, Image &binary);
    Pixel comp(const Pixel &pixBilateral, const Pixel &pixBinary);
};

#endif
