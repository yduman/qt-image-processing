#ifndef LUCY_H
#define LUCY_H

#include "Algorithm.h"

class Lucy : public Algorithm {
public:
    void process(const Parameters &params, const Image &src, Image &dst);
    
};

#endif
