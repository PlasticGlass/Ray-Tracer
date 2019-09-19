#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"
#include "structs.h"

using namespace std;

Vec3f reflect(const Vec3f &I, const Vec3f &N) {
    return I - N*2.f*(I*N);
}

Vec3f cast_ray(Ray& r, vector<Sphere> spheres, vector<Light> lights){
    
    for(auto s : spheres){
        Intersection intersection_point;
        if(s.intersect(r, intersection_point)){
            float diffuse = 0;
            float specular = 0;

            for(auto l : lights){
                //Apply lambertian/cosine shading + phong reflection model
                Vec3f point_normal = intersection_point.normal;
                Vec3f light_direction = (l.position-intersection_point.ray.point_at_time(intersection_point.time)).normalize();
                Vec3f reflected = (2.0*(dot(light_direction, point_normal))*point_normal - light_direction).normalize();

                float shade = dot(point_normal, light_direction);
                float specular_reflection = dot(reflected, -1*r.direction);

                if(shade < 0){
                    shade = 0.0;
                }

                if(specular_reflection < 0){
                    specular_reflection = 0;
                }

                specular_reflection = powf(specular_reflection, s.material.shininess)*s.material.specular;

                diffuse += (shade*l.intensity);
                specular += (specular_reflection*l.intensity);
            }

            return (s.material.colour * (diffuse + s.material.ambient) + Vec3f(1,1,1)*specular);
        }
    } 
    
    return Vec3f(0.298, 0.7058, 0.9843);
}



void render(vector<Vec3f>& framebuffer, const int height, const int width, vector<Sphere> spheres, vector<Light> lights) {
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

            framebuffer[i+j*width] = cast_ray(r, spheres, lights);
            
        }
    }
}

void write_image_to_file(vector<Vec3f>& framebuffer, const int height, const int width) {
    ofstream ofs ("image.ppm"); // save the framebuffer to file
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < height*width; ++i) {
        Vec3f& v = framebuffer[i];
        float max = std::max(v[0], std::max(v[1], v[2]));
        if (max>1) v = v*(1/max);
        for (int j = 0; j<3; j++) { //Write out 3 components (rgb) of Vec3 (pixel) at index i in framebuffer
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    const int width    = 1280;
    const int height   = 720;
    vector<Vec3f> framebuffer(width*height); //List of Vec3
    vector<Sphere> spheres;

    Material red(Vec3f(0.3, 0.1, 0.1), 0.8, 0.3, 10);
    Material mat(Vec3f(0.4, 0.4, 0.3), 0.8, 0.1, 50);

    vector<Light> lights;
    //lights.push_back(Light(Vec3f(-80, 40, -40), 1.5));
    lights.push_back(Light(Vec3f(-20, 20,  20), 1.5));
    lights.push_back(Light(Vec3f( 30, 50, -25), 1.8));
    lights.push_back(Light(Vec3f( 30, 20,  30), 1.7));

    Sphere s(Vec3f(0,10,-30), 1, red);
    Sphere p(Vec3f(5,0,-30), 1, mat);
    Sphere q(Vec3f(-5,0,-30), 6, red);
    Sphere r(Vec3f(-10,10,-30), 3, mat);

    spheres.push_back(s);
    spheres.push_back(p);
    spheres.push_back(q);
    spheres.push_back(r);

    render(framebuffer, height, width, spheres, lights);
    write_image_to_file(framebuffer, height, width);

    return 0;
}