#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

struct Ray { //Ray represented by equation of line: origin + direction*t
    Vec3f origin;
    Vec3f direction;

    Ray(Vec3f& o, Vec3f& d) {
        origin = o;
        direction = d;
    }

    Ray(const Vec3f& o, const Vec3f& d) {
        origin = o;
        direction = d;
    }
};

struct Sphere {
    Vec3f center;
    float radius;

    Sphere(const Vec3f& c, float r) {
        center = c;
        radius = r;
    }

    Sphere(Vec3f& c, float r) {
        center = c;
        radius = r;
    }

    bool intersect(Ray& r){
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
        float b = 2*dot(r.origin - center, r.direction);
        float c = dot(r.origin - center, r.origin - center) - radius*radius;

        float t1 = (-b + sqrtf(b*b - 4*a*c))/(2*a);
        float t2 = (-b - sqrtf(b*b - 4*a*c))/(2*a);

        return (t1 > 0 || t2 > 0);
    }
};

Vec3f cast_ray(Ray& r, Sphere& s){
    if(s.intersect(r)){
        return Vec3f(0.9, 0.9, 0.6);
    } else {
        return Vec3f(0.298, 0.7058, 0.9843);
    }
}

void render(std::vector<Vec3f>& framebuffer, const int height, const int width, Sphere& sphere) {
    float widthf = (float)width;
    float heightf = (float)height;
    
    for (int j = 0; j<height; j++) {
        for (int i = 0; i<width; i++) {
            int fov = M_PI/2;
            float tan_val = tan(fov/2.0);
            float ar = widthf/heightf;

            //(i+0.5)/width which horizontal pixel we're on
            //2*((i+0.5)/width) - 1 re-maps the horizontal coordinate from [0,1] to [-1,1]
            //make y coordinate negative otherwise will be flipped
            //multiply by aspect ratio to make square again
            //multiply by tan_val to zoom in or out based on fov
            //tan(fov/2) is distance from center of screen to edge

            float x =  (2*(i+0.5)/widthf  - 1) * ar * tan_val; //Adding 0.5 to pixel value because we want ray to pass through "middle" of pixel, not its edge
            float y = -(2*(j+0.5)/heightf - 1) * tan_val;
            float z = -1; //z coordinate is -1, so "screen" (stuff thats visible) is 1 unit away from camera

            Ray r(Vec3f(0,0,0), Vec3f(x, y, z)); 
            framebuffer[i+j*width] = cast_ray(r, sphere);
        }
    }
}

void write_image_to_file(std::vector<Vec3f>& framebuffer, const int height, const int width) {
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
    const int width    = 1920;
    const int height   = 1080;
    std::vector<Vec3f> framebuffer(width*height); //List of Vec3
    Sphere s(Vec3f(-3,0,-16), 2);
    render(framebuffer, height, width, s);
    write_image_to_file(framebuffer, height, width);

    return 0;
}