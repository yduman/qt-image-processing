#include "LucyOMP.h"

#include <omp.h>

void LucyOMP::process(const Parameters &params, const Image &src, Image &dst)
{
   clock_t start, end, runtime;
   float duration;
   start = clock();

   dst = src;

   const int n = params.lucyIterations;
   const int height = src.height();
   const int width = src.width();
   const int k = 51;
   const int range = (51 - 1) / 2;
   Image u = Image(height, width);
   Image b = Image(height, width);
   Image d = Image(height, width);

   for (int i = 1; i <= n; i++)
   {
      printf("Iteration %i of %i\n", i, n);
      double red, green, blue;
      double new_red, new_green, new_blue;

      // -------------------- FILTER -------------------- //
      for (int y = 0; y < height; y++) {
         for (int x = 0; x < width; x++) {
            new_red = 0.0;
            new_green = 0.0;
            new_blue = 0.0;

            for (int j = x - range; j <= x + range; j++) {
               if (j < 0 || j >= width) {
                  red = 255.0;
                  green = 255.0;
                  blue = 255.0;
               } else {
                  const Pixel &pixel = dst[y][j];
                  red = pixel.r;
                  green = pixel.g;
                  blue = pixel.b;
               }

               new_red += red;
               new_green += green;
               new_blue += blue;
            }

            new_red = new_red / k;
            new_blue = new_blue / k;
            new_green = new_green / k;
            b[y][x] = Pixel(new_red, new_green, new_blue);
         }
      }

      // -------------------- DIVISION -------------------- //
      for (int y = 0; y < height; y++) {
         for (int x = 0; x < width; x++) {
            const Pixel &f1 = src[y][x];
            const Pixel &f2 = b[y][x];

            red = (f1.r / f2.r > 255) ? 255 : f1.r / f2.r;
            green = (f1.g / f2.g > 255) ? 255 : f1.g / f2.g;
            blue = (f1.b / f2.b > 255) ? 255 : f1.b / f2.b;
            u[y][x] = Pixel(red, green, blue);
         }
      }

      // -------------------- FILTER -------------------- //
      for (int y = 0; y < height; y++) {
         for (int x = 0; x < width; x++) {
            new_red = 0.0;
            new_green = 0.0;
            new_blue = 0.0;

            for (int j = x - range; j <= x + range; j++) {
               if (j < 0 || j >= width) {
                  red = 255.0;
                  green = 255.0;
                  blue = 255.0;
               } else {
                  const Pixel &pixel = u[y][j];
                  red = pixel.r;
                  green = pixel.g;
                  blue = pixel.b;
               }

               new_red += red;
               new_green += green;
               new_blue += blue;
            }

            new_red = new_red / k;
            new_blue = new_blue / k;
            new_green = new_green / k;
            d[y][x] = Pixel(new_red, new_green, new_blue);
         }
      }

      // -------------------- MULTIPLICATION -------------------- //
      for (int y = 0; y < height; y++) {
         for (int x = 0; x < width; x++) {
            const Pixel &f1 = dst[y][x];
            const Pixel &f2 = d[y][x];

            red = (f1.r * f2.r > 255) ? 255 : f1.r * f2.r;
            green = (f1.g * f2.g > 255) ? 255 : f1.g * f2.g;
            blue = (f1.b * f2.b > 255) ? 255 : f1.b * f2.b;
            dst[y][x] = Pixel(red, green, blue);
         }
      }
   }

   end = clock();
   runtime = end - start;
   duration = 1000 * (float)runtime / CLOCKS_PER_SEC;
   printf("Dauer: %f ms\n", duration);
}