#ifndef CARTOONIZE_H
#define CARTOONIZE_H

#include "Algorithm.h"

// Autoren: Yildiz Kasimay, Artjom Poljakow, Yadullah Duman

class Cartoonize : public Algorithm
{
public:
    void process(const Parameters &params, const Image &src, Image &dst);

    double** createMatrix(int height, int width);

    // Bilateral Filter
    Image bilFilter(const Image &img);
    Pixel calcPixelBilFilter(const Image &img, int y, int x);
    bool isInRange(int size, int y, int x, const int height, const int width);
    double weight(const Image &img, int y, int x, int k, int l);
    double cDomain(int y, int x, int k, int l);
    double cEdge(const Image &img, int y, int x, int k, int l);
    double** initWeightKernel(const Image &img, int y, int x);
    double euklid(int r, int g, int b);

    // Binary Filter
    Image binFilter(const Image &img, char op);
    Image binImage(const Image &img);
    Image edgeStrength(Image &imgX, Image &imgY);
    Image edgeDetection(Image &img);
    Pixel calcPixelBinFilter(const Image &img, int y, int x, char op);
    Pixel calcEdgeStrength(int y, int x, Image &imgX, Image &imgY);
    Pixel calcEdgeDetection(int y, int x, Image &img);
    double** getSobelKernel(char c);

    // Cartoonize Filter
    Image cartoonizeImage(Image &bilateral, Image &binary);
    Pixel calcPixelCartFilter(const Pixel &pixBilateral, const Pixel &pixBinary);
};

#endif
