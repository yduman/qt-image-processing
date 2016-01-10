#include "LucyOMP.h"

#include <chrono>
#include <iostream>

// Autoren: Yildiz Kasimay, Artjom Poljakow, Yadullah Duman

using namespace std;
using namespace std::chrono;

void LucyOMP::process(const Parameters &params, const Image &src, Image &dst)
{
    // ermittle Startzeit fuer Laufzeitmessung
    auto t1 = high_resolution_clock::now();

    dst = src;

    // initialisiere noetigen Parameter
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
        #pragma omp parallel for
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (!isInRange(x, width)) {
                    b[y][x] = Pixel(255.0 / k, 255.0 / k, 255.0 / k);
                    continue;
                }
                b[y][x] = calcPixel(k, dst, y, x);
            }
        }

        // -------------------- DIVISION -------------------- //
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const Pixel &dividend = src[y][x];
                const Pixel &divisor = b[y][x];

                red   = (dividend.r / divisor.r > 255) ? 255 : dividend.r / divisor.r;
                green = (dividend.g / divisor.g > 255) ? 255 : dividend.g / divisor.g;
                blue  = (dividend.b / divisor.b > 255) ? 255 : dividend.b / divisor.b;
                u[y][x] = Pixel(red, green, blue);
            }
        }

        // -------------------- FILTER -------------------- //
        #pragma omp parallel for
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (!isInRange(x, width)) {
                    d[y][x] = Pixel(255.0 / k, 255.0 / k, 255.0 / k);
                    continue;
                }
                d[y][x] = calcPixel(k, u, y, x);
            }
        }

        // -------------------- MULTIPLICATION -------------------- //
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const Pixel &multiplikand = dst[y][x];
                const Pixel &multiplikator = d[y][x];

                red   = (multiplikand.r * multiplikator.r > 255) ? 255 : multiplikand.r * multiplikator.r;
                green = (multiplikand.g * multiplikator.g > 255) ? 255 : multiplikand.g * multiplikator.g;
                blue  = (multiplikand.b * multiplikator.b > 255) ? 255 : multiplikand.b * multiplikator.b;
                dst[y][x] = Pixel(red, green, blue);
            }
        }
    }

    // Laufzeitmessung = Endzeit - Startzeit
    auto t2 = high_resolution_clock::now();
    auto elapsed = duration_cast<milliseconds>(t2 - t1);
    printf("\n *********** FINISHED *********** \n");
    printf("ALGORITHM: RICHARDSON-LUCY with OpenMP\n");
    cout << "RUNTIME: " << elapsed.count() << " ms" << endl;
    cout << endl;
}

/**
 * prueft, ob wir noch in der gueltigen Range sind, d.h.
 * ob links und rechts vom Pixel noch 25 weitere Pixel sind
 */
bool LucyOMP::isInRange(int i, const int width) {
    return !(i - 25 < 0 || i + 25 > width);
}

/**
 * wendet den Kernel auf den Pixel an
 */
Pixel LucyOMP::calcPixel(double k, const Image &img, int y, int x) {
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