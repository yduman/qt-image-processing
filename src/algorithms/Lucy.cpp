#include "Lucy.h"

#include <chrono>
#include <iostream>


void Lucy::process(const Parameters &params, const Image &src, Image &dst)
{

    auto t1 = std::chrono::high_resolution_clock::now();

    dst = src;
    const int n = params.lucyIterations;
    const int height = src.height();
    const int width = src.width();
    const int k = 51;
    Image u = Image(height, width);
    Image b = Image(height, width);
    Image d = Image(height, width);
    double red, green, blue;

    for (int i = 1; i <= n; i++)
    {
        printf("Iteration %i of %i\n", i, n);

        // -------------------- FILTER -------------------- //
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (!isInRange(x, width)) {
                    b[y][x] = Pixel(255.0 / k, 255.0 / k, 255.0 / k);
                    continue;
                }
                b[y][x] = kernelOp(k, dst, y, x);

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
                if (!isInRange(x, width)) {
                    d[y][x] = Pixel(255.0 / k, 255.0 / k, 255.0 / k);
                    continue;
                }
                d[y][x] = kernelOp(k, u, y, x);
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

    auto t2 = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    printf("\n *********** FINISHED *********** \n");
    printf("ALGORITHM: RICHARDSON-LUCY\n");
    std::cout << "RUNTIME: " << elapsed.count() << " ms" << std::endl;
}

bool Lucy::isInRange(int i, const int width) {
    return !(i - 25 < 0 || i + 25 > width);
}

Pixel Lucy::kernelOp(double k, const Image &img, int y, int x) {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    for (int i = x - 25; i < x + 25; ++i) {
        const Pixel &pixel = img[y][i];
        r += pixel.r / k;
        g += pixel.g / k;
        b += pixel.b / k;
    }
    return Pixel(r, g, b);
}