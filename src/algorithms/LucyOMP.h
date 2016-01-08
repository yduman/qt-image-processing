#ifndef LUCYOMP_H
#define LUCYOMP_H

#include "Algorithm.h"

class LucyOMP : public Algorithm
{
public:
   void process(const Parameters &params, const Image &src, Image &dst);

private:
    bool isInRange(int i, const int width);
    Pixel kernelOp(double k, const Image &img, int y, int x);
};

#endif
