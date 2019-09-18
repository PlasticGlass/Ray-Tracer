#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"
#include "structs.h"

using namespace std;

Vec3f cast_ray(Ray& r, vector<Sphere> spheres, vector<Light> lights){
    Intersection intersection_point;
    for(auto s : spheres){
        if(s.intersect(r, intersection_point)){
            float total_coeff = 0;
            //float ambient_coefficient = 0.2;
            float diffuse_coefficient = 0;
            //Vec3f col(s.material.colour.x, s.material.colour.y,s.material.colour.z);

            for(auto l : lights){
                //Apply lambertian/cosine shading
                //float shade = dot(
                    //(l.position-intersection_point).normalize(),
                  //  intersection_point.normalize());

                float shade = dot(intersection_point.normal,(l.position-s.center));
                //diffuse_coefficient += ;

               // if(shade < 0){
                 //   shade = 0.0;
               // }

                total_coeff += l.intensity*shade;
            }

            return s.material.colour * total_coeff;
        }
    } 
    
    return Vec3f(0.298, 0.7058, 0.9843);
}

void render(vector<Vec3f>& framebuffer, const int height, const int width, vector<Sphere> spheres, vector<Light> lights) {
    float widthf = (float)width;
    float heightf = (float)height;
    
    for (int j = 0; j<height; j++) {
        for (int i = 0; i<width; i++) {
            int fov = M_PI/3;
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

            framebuffer[i+j*width] = cast_ray(r, spheres, lights);
            
        }
    }
}

void write_image_to_file(vector<Vec3f>& framebuffer, const int height, const int width) {
    ofstream ofs ("image.ppm"); // save the framebuffer to file
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < height*width; ++i) {
        for (int j = 0; j<3; j++) { //Write out 3 components (rgb) of Vec3 (pixel) at index i in framebuffer
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    const int width    = 1024;
    const int height   = 768;
    vector<Vec3f> framebuffer(width*height); //List of Vec3
    vector<Sphere> spheres;

    Material red(Vec3f(0.3, 0.1, 0.1));
    Material mat;

    vector<Light> lights;
    lights.push_back(Light(Vec3f(-20,20,20), 1.5));

    Sphere s(Vec3f(0,10,-30), 1, red);
    Sphere p(Vec3f(5,0,-30), 1, mat);
    Sphere q(Vec3f(-5,0,-30), 6, red);
    Sphere r(Vec3f(-10,10,-30), 4, mat);

    spheres.push_back(s);
    spheres.push_back(p);
    spheres.push_back(q);
    spheres.push_back(r);

    render(framebuffer, height, width, spheres, lights);
    
    
    write_image_to_file(framebuffer, height, width);

    return 0;
}