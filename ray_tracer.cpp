#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

struct Ray { //Ray represented by equation of line: origin + direction*t
    Vec3f origin;
    Vec3f direction;
    float t;

    Ray(Vec3f& o, Vec3f& d, float& t0) {
        origin = o;
        direction = d;
        t = t0;
    }
};

struct Sphere {
    Vec3f center;
    float radius;

    Sphere(Vec3f& c, float r) {
        center = c;
        radius = r;
    }

    bool intersect_sphere(Ray& r){
        //Represent ray-sphere combination as implicit function and apply quadratic formula to find points of intersection (roots)
        //(ray-center).(ray-center)-radius^2=0
        //(o + dt - c).(o + dt - c) - r^2 = 0
        //((o-c) + dt).((o-c) + dt) - r^2 = 0
        //(o-c).(o-c) + 2t((o-c).d) + t^2*(d.d) - r^2 = 0 - quadratic eqn in t
        // t = (-b +- sqrt(b^2 - 4ac))/2a
        // a = d.d
        // b = (o-c).d
        // c = (o-c).(o-c) - r^2

        float a = dot(r.direction, r.direction);
        float b = dot(r.origin - center, r.direction);
        float c = dot(r.origin - center, r.origin - center);

        float t1 = (-b + sqrt(b*b - 4*a*c))/(2*a);
        float t2 = (-b - sqrt(b*b - 4*a*c))/(2*a);

        return (t1 > 0 || t2 > 0);
    }
};

void render() {
    const int width    = 1024;
    const int height   = 768;
    std::vector<Vec3f> framebuffer(width*height); //List of Vec3

    for (int j = 0; j<height; j++) {
        for (int i = 0; i<width; i++) {
            framebuffer[i+j*width] = Vec3f(
                                        j/float(height),
                                        i/float(width), 
                                        0);
        }
    }

    std::ofstream ofs ("image.ppm"); // save the framebuffer to file
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < height*width; ++i) {
        for (int j = 0; j<3; j++) { //Write out 3 components (rgb) of Vec3 (pixel) at index i in framebuffer
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    render();
    return 0;
}