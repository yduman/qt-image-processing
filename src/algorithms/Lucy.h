#ifndef LUCY_H
#define LUCY_H

#include "Algorithm.h"

// Autoren: Yildiz Kasimay, Artjom Poljakow, Yadullah Duman

class Lucy : public Algorithm {
public:
    void process(const Parameters &params, const Image &src, Image &dst);

private:
   bool isInRange(int i, const int width);
   Pixel calcPixel(double k, const Image &img, int y, int x);
};

#endif
