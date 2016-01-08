#include "Cartoonize.h"

#include <ctime>

using namespace std;

int sigma_r;
int sigma_d;
int kernelSize;
int tau;

//************************************************************************
// process
//************************************************************************
void Cartoonize::process(const Parameters &params, const Image &src, Image &dst)
{
   clock_t start, end, runtime;
   start = clock();
   float duration;

   // get vars
   sigma_r = params.sigma_r;
   sigma_d = params.sigma_d;
   kernelSize = params.kernelSize;
   tau = params.tau;

   dst = src;

   Image bilateral = bilFilter(src);
   Image binary = binTraverse(src);
   dst = cartFilter(bilateral, binary);

   end = clock();
   runtime = end - start;
   duration = 1000 * (float)runtime / CLOCKS_PER_SEC;

   printf("Dauer: %f ms\n", duration);
}

double** Cartoonize::createMatrix(int height, int width) {
   double* row = (double*)calloc(height * width, sizeof(double));
   double** ret = (double**)calloc(height, sizeof(double));

   for (int i = 0; i < height; ++i) {
      ret[i] = row + i * width;
   }

   return ret;
}

void Cartoonize::destroyMatrix(double **matrix) {
   free(*matrix);
   free(matrix);
}

//************************************************************************
// BILATERAL
//************************************************************************

double Cartoonize::euklid(int r, int g, int b) {
   return sqrt(pow(r, 2) + pow(g, 2) + pow(b, 2));
}

double Cartoonize::calcDomain(int y, int x, int k, int l) {
   double summand_lhs = pow((y - k), 2);
   double summand_rhs = pow((x - l), 2);
   double dividend = 2 * pow(sigma_d, 2);
   double result = -(summand_lhs + summand_rhs) / dividend;

   return exp(result);
}

double Cartoonize::calcEdge(const Image &img, int y, int x, int k, int l) {
   double r, g, b;
   const Pixel &pix1 = img[y][x];
   const Pixel &pix2 = img[k][l];

   r = pix1.r - pix2.r;
   g = pix1.g - pix2.g;
   b = pix1.b - pix2.b;

   double numerator = pow(euklid(r, g, b), 2);
   double denominator = 2 * pow(sigma_r, 2);
   double result = -(numerator)/denominator;

   return exp(result);
}

double Cartoonize::calcKernelField(const Image &img, int y, int x, int k, int l) {
   return calcDomain(y, x, k, l) * calcEdge(img, y, x, k, l);
}

bool Cartoonize::isInRange(int size, int y, int x, const int height, const int width) {
   return !((y - size / 2 < 0 || y + size / 2 >= height) || (x - size / 2 < 0 || x + size / 2 >= width));
}

double** Cartoonize::initKernel(const Image &img, int y, int x) {
   int height = 0;
   int width = 0;
   int range = kernelSize/2;

   double** kernel = createMatrix(kernelSize, kernelSize);

   for (int i = y - range; i < y + range; ++i) {
      for (int j = x - range; j < x + range; ++j) {
         kernel[height][width] = calcKernelField(img, y, x, i, j);
         width++;
      }
      width = 0;
      height++;
   }
   return kernel;
}

Pixel Cartoonize::bilKernel(const Image &img, int y, int x) {
   double** kernel = initKernel(img, y, x);
   double numerator_R = 0.0;
   double numerator_G = 0.0;
   double numerator_B = 0.0;
   double denominator = 0.0;
   int height = 0;
   int width = 0;
   int range = kernelSize / 2;

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

Image Cartoonize::bilFilter(const Image &img) {
   const int height = img.height();
   const int width = img.width();
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         if (!isInRange(kernelSize, y, x, height, width)) {
            result[y][x] = img[y][x];
            continue;
         } else {
            result[y][x] = bilKernel(img, y, x);
         }
      }
   }

   return result;
}

//************************************************************************
// BINARY
//************************************************************************

Pixel Cartoonize::calcEdgeStrength(int y, int x, Image &imgX, Image &imgY) {
   double r, g, b;
   const Pixel &pix_X = imgX[y][x];
   const Pixel &pix_Y = imgY[y][x];

   r = sqrt(pow(pix_X.r, 2) + pow(pix_Y.r, 2));
   g = sqrt(pow(pix_X.g, 2) + pow(pix_Y.g, 2));
   b = sqrt(pow(pix_X.b, 2) + pow(pix_Y.b, 2));

   r = adjustPixelCol(r);
   g = adjustPixelCol(g);
   b = adjustPixelCol(b);

   return Pixel(r, g, b);
}

Pixel Cartoonize::calcEdgeDetection(int y, int x, Image &img) {
   double r, g, b;
   const Pixel &pix = img[y][x];

   r = tau >= pix.r ? pix.r : 0.0;
   g = tau >= pix.g ? pix.g : 0.0;
   b = tau >= pix.b ? pix.b : 0.0;

   return Pixel(r, g, b);
}

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

double** Cartoonize::getBlurKernel() {
   double** kernel = createMatrix(3, 3);

   kernel[0][0] = 1.0 / 16.0; kernel[0][1] = 2.0 / 16.0; kernel[0][2] = 1.0 / 16.0;
   kernel[1][0] = 2.0 / 16.0; kernel[1][1] = 4.0 / 16.0; kernel[1][2] = 2.0 / 16.0;
   kernel[2][0] = 1.0 / 16.0; kernel[2][1] = 2.0 / 16.0; kernel[2][2] = 1.0 / 16.0;

   return kernel;
}

double Cartoonize::adjustPixelCol(double pixel) {
   if (pixel > 255.0)
      return 255.0;

   if (pixel < 0.0)
      return 0.0;

   return pixel;
}

Pixel Cartoonize::binKernel(const Image &img, int y, int x, char op) {
   int height = 0;
   int width  = 0;
   double r = 0.0;
   double g = 0.0;
   double b = 0.0;
   double** kernel;

   switch (op) {
      case 'b': kernel = getBlurKernel();
           break;
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

   r = adjustPixelCol(r);
   g = adjustPixelCol(g);
   b = adjustPixelCol(b);

   return Pixel(r, g, b);
}

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
            result[y][x] = binKernel(img, y, x, op);
         }
      }
   }
   return result;
}

Image Cartoonize::binTraverse(const Image &img) {
//   Image g;
   Image gX;
   Image gY;
   Image G;
   Image G_t;

//   g   = binFilter(img, 'b');    // blurring --> 'b'
   gX  = binFilter(img, 'x');      // Sobel_X
   gY  = binFilter(img, 'y');      // Sobel Y
   G   = edgeStrength(gX, gY);
   G_t = edgeDetection(G);

   return G_t;
}

//************************************************************************
// CARTOONIZE
//************************************************************************

Pixel Cartoonize::comp(const Pixel &pixBilateral, const Pixel &pixBinary) {
   double r, g, b;
   r = pixBinary.r;
   g = pixBinary.g;
   b = pixBinary.b;

   if (r == 0.0 && g == 0.0 && b == 0.0)
      return Pixel(r, g, b);
   else
      return pixBilateral;
}

Image Cartoonize::cartFilter(Image &bilateral, Image &binary) {
   const int height = min(bilateral.height(), binary.height());
   const int width = min(bilateral.width(), binary.width());
   Image result = Image(height, width);

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         const Pixel &pixBil = bilateral[y][x];
         const Pixel &pixBin = binary[y][x];
         result[y][x] = comp(pixBil, pixBin);
      }
   }
   return result;
}