#ifndef LUCYOMP_H
#define LUCYOMP_H

#include "Algorithm.h"

// Autoren: Yildiz Kasimay, Artjom Poljakow, Yadullah Duman

class LucyOMP : public Algorithm
{
public:
   void process(const Parameters &params, const Image &src, Image &dst);

private:
    bool isInRange(int i, const int width);
    Pixel calcPixel(double k, const Image &img, int y, int x);
};

#endif
