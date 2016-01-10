#include "Cartoonize.h"

#include <chrono>
#include <iostream>

// Autoren: Yildiz Kasimay, Artjom Poljakow, Yadullah Duman

using namespace std;
using namespace std::chrono;

int sig_r;
int sig_d;
int kSize;
int t;

//************************************************************************
// process
//************************************************************************
void Cartoonize::process(const Parameters &params, const Image &src, Image &dst)
{
   // ermittle Startzeit
   auto t1 = high_resolution_clock::now();

   // initialisiere Parameter
   sig_r = params.sigma_r;
   sig_d = params.sigma_d;
   kSize = params.kernelSize;
   t = params.tau;

   dst = src;

   // Algorithmus
   Image bilateral = bilFilter(src);
   Image binary = binImage(bilateral);
   dst = cartoonizeImage(bilateral, binary);

   // berechne Laufzeit = Endzeit - Startzeit
   auto t2 = high_resolution_clock::now();
   auto elapsed = duration_cast<milliseconds>(t2 - t1);
   printf("\n *********** FINISHED *********** \n");
   printf("ALGORITHM: CARTOONIZE\n");
   cout << "RUNTIME: " << elapsed.count() << " ms" << endl;
   cout << endl;
}

//************************************************************************
// BILATERAL-FILTER
//************************************************************************

/**
 * Hilfsmethode um eine Kernelmatrix zu generieren
 */
double** Cartoonize::createMatrix(int height, int width) {
    double* row = (double*)calloc(height * width, sizeof(double));
    double** ret = (double**)calloc(height, sizeof(double));

    for (int i = 0; i < height; ++i) {
        ret[i] = row + i * width;
    }

    return ret;
}

/**
 * euklidische Norm
 */
double Cartoonize::euklid(int r, int g, int b) {
   return sqrt(pow(r, 2) + pow(g, 2) + pow(b, 2));
}

/**
 * cDomain Formel
 * y = i
 * x = j
 */
double Cartoonize::cDomain(int y, int x, int k, int l) {
   double summand_lhs = pow((y - k), 2);    // (i - k)²
   double summand_rhs = pow((x - l), 2);    // (j - l)²
   double denominator = 2 * pow(sig_d, 2);  // Nenner
   double result = -(summand_lhs + summand_rhs) / denominator;

   return exp(result);
}

/**
 * cEdge Formel
 * y = i
 * x = j
 */
double Cartoonize::cEdge(const Image &img, int y, int x, int k, int l) {
   double r, g, b;
   const Pixel &f_ij = img[y][x];
   const Pixel &f_kl = img[k][l];

   r = f_ij.r - f_kl.r;
   g = f_ij.g - f_kl.g;
   b = f_ij.b - f_kl.b;

   double numerator = pow(euklid(r, g, b), 2);  // ||f(i,j) - f(k,l)||²
   double denominator = 2 * pow(sig_r, 2);      // Nenner
   double result = -(numerator)/denominator;

   return exp(result);
}

/**
 * w_ijkl Formel
 */
double Cartoonize::weight(const Image &img, int y, int x, int k, int l) {
   return cDomain(y, x, k, l) * cEdge(img, y, x, k, l);
}

/**
 * prueft, ob wir noch in der gueltigen Range sind
 */
bool Cartoonize::isInRange(int size, int y, int x, const int height, const int width) {
   return !((y - size / 2 < 0 || y + size / 2 >= height) || (x - size / 2 < 0 || x + size / 2 >= width));
}

/**
 * initialisiere w_ijkl-Kernel
 */
double** Cartoonize::initWeightKernel(const Image &img, int y, int x) {
   int height = 0;
   int width = 0;
   int range = kSize / 2;

   double** kernel = createMatrix(kSize, kSize);

   for (int i = y - range; i < y + range; ++i) {
      for (int j = x - range; j < x + range; ++j) {
         kernel[height][width] = weight(img, y, x, i, j);
         width++;
      }
      width = 0;
      height++;
   }
   return kernel;
}

/**
 * Berechnung eines Ausgabepixels beim bilateralen Filtern
 * g_ij-Kernel
 */
Pixel Cartoonize::calcPixelBilFilter(const Image &img, int y, int x) {
    double** kernel = initWeightKernel(img, y, x);

    double numerator_R = 0.0;
    double numerator_G = 0.0;
    double numerator_B = 0.0;

    double denominator = 0.0;

    int height = 0;
    int width = 0;
    int range = kSize / 2;

    for (int i = y - range; i < y + range; ++i) {
       for (int j = x - range; j < x + range; ++j) {
           const Pixel &pix = img[i][j];

           numerator_R += kernel[height][width] * pix.r;
           numerator_G += kernel[height][width] * pix.g;
           numerator_B += kernel[height][width] * pix.b;
           denominator += kernel[height][width];

           width++;
       }
        width = 0;
        height++;
    }
    return Pixel(numerator_R/denominator, numerator_G/denominator, numerator_B/denominator);
}

/**
 * Prozess des Bilateralfilters
 */
Image Cartoonize::bilFilter(const Image &img) {
   const int height = img.height();
   const int width = img.width();
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         if (!isInRange(kSize, y, x, height, width)) {
            result[y][x] = img[y][x];
            continue;
         } else {
            result[y][x] = calcPixelBilFilter(img, y, x);
         }
      }
   }

   return result;
}

//************************************************************************
// BINARY-FILTER
//************************************************************************

/**
 * Berechnung der Kantenstaerke pixelweise
 */
Pixel Cartoonize::calcEdgeStrength(int y, int x, Image &imgX, Image &imgY) {
   double r, g, b;
   const Pixel &pix_X = imgX[y][x];
   const Pixel &pix_Y = imgY[y][x];

   r = sqrt(pow(pix_X.r, 2) + pow(pix_Y.r, 2));
   g = sqrt(pow(pix_X.g, 2) + pow(pix_Y.g, 2));
   b = sqrt(pow(pix_X.b, 2) + pow(pix_Y.b, 2));

   return Pixel(r, g, b);
}

/**
 * Berechnung der Kantendetektion pixelweise
 */
Pixel Cartoonize::calcEdgeDetection(int y, int x, Image &img) {
   double r, g, b;
   const Pixel &pix = img[y][x];

   r = t >= pix.r ? pix.r : 0.0;
   g = t >= pix.g ? pix.g : 0.0;
   b = t >= pix.b ? pix.b : 0.0;

   return Pixel(r, g, b);
}

/**
 * Berechnung der Kantenstaerke
 */
Image Cartoonize::edgeStrength(Image &imgX, Image &imgY) {
   const int height = min(imgX.height(), imgY.height());
   const int width  = min(imgX.width(), imgY.width());
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         result[y][x] = calcEdgeStrength(y, x, imgX, imgY);
      }
   }
   return result;
}

/**
 * Berechnung der Kantendetektion
 */
Image Cartoonize::edgeDetection(Image &img) {
   const int height = img.height();
   const int width = img.width();
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         result[y][x] = calcEdgeDetection(y, x, img);
      }
   }
   return result;
}

/**
 * getter fuer den Sobelkernel in x- und y-Richtung
 */
double** Cartoonize::getSobelKernel(char c) {
   double** kernel = createMatrix(3, 3);

   if (c == 'x')
   {
      kernel[0][0] = 1.0; kernel[0][1] = 0.0; kernel[0][2] = -1.0;
      kernel[1][0] = 2.0; kernel[1][1] = 0.0; kernel[1][2] = -2.0;
      kernel[2][0] = 1.0; kernel[2][1] = 0.0; kernel[2][2] = -1.0;
   }
   else
   {
      kernel[0][0] =  1.0; kernel[0][1] =  2.0; kernel[0][2] =  1.0;
      kernel[1][0] =  0.0; kernel[1][1] =  0.0; kernel[1][2] =  0.0;
      kernel[2][0] = -1.0; kernel[2][1] = -2.0; kernel[2][2] = -1.0;
   }

   return kernel;
}

/**
 * Berechnung eines Ausgabepixel fuer das Binaerkantenbild
 */
Pixel Cartoonize::calcPixelBinFilter(const Image &img, int y, int x, char op) {
    int height = 0;
    int width  = 0;

    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double** kernel;

    switch (op) {
       case 'x': kernel = getSobelKernel('x');
            break;
       case 'y': kernel = getSobelKernel('y');
            break;
       default: printf("Error in function \"binKernel\" --> input char: %c\n", op);
    }

    for (int i = y - 1; i <= y + 1; ++i) {
       for (int j = x - 1; j <= x + 1; ++j) {
          const Pixel &pix = img[i][j];
          r += kernel[height][width] * pix.r;
          g += kernel[height][width] * pix.g;
          b += kernel[height][width] * pix.b;

          width++;
       }
       width = 0;
       height++;
    }
    return Pixel(r, g, b);
}

/**
 * Binaerbildfilter
 */
Image Cartoonize::binFilter(const Image &img, char op) {
   const int height = img.height();
   const int width = img.width();
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         if (!isInRange(3, y, x, height, width)) {
            result[y][x] = img[y][x];
            continue;
         } else {
            result[y][x] = calcPixelBinFilter(img, y, x, op);
         }
      }
   }
   return result;
}

/**
 * Generierung eines Binaerkantenbildes
 */
Image Cartoonize::binImage(const Image &img) {
   Image gX;
   Image gY;
   Image G;
   Image G_t;

   gX  = binFilter(img, 'x');   // Binaerfilter mit Sobelkernel in x-Richung
   gY  = binFilter(img, 'y');   // Binaerfilter mit Sobelkernel in y-Richung
   G   = edgeStrength(gX, gY);  // Berechnung der Kantenstaerke
   G_t = edgeDetection(G);      // Berechnung der Kantendetektion

   return G_t;
}

//************************************************************************
// CARTOONIZE-FILTER
//************************************************************************

/**
 * Alle Kanten des Binaerkantenbildes werden direkt in das bilateral
 * gefilterte Bild ueberfuehrt, d.h.
 * jeder zu einer Kante gehoerende Pixel in der Maske des Binaerkantenbildes,
 * wird im bilateral gefilterten Bild auf schwarz gesetzt
 */
Pixel Cartoonize::calcPixelCartFilter(const Pixel &pixBilateral, const Pixel &pixBinary) {
   double r, g, b;
   r = pixBinary.r;
   g = pixBinary.g;
   b = pixBinary.b;

   if (r == 0.0 && g == 0.0 && b == 0.0)
      return Pixel(r, g, b);
   else
      return pixBilateral;
}

Image Cartoonize::cartoonizeImage(Image &bilateral, Image &binary) {
   const int height = min(bilateral.height(), binary.height());
   const int width = min(bilateral.width(), binary.width());
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         const Pixel &pixBil = bilateral[y][x];
         const Pixel &pixBin = binary[y][x];
         result[y][x] = calcPixelCartFilter(pixBil, pixBin);
      }
   }
   return result;
}