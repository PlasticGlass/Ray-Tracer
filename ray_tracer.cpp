#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"
#include "structs.h"

using namespace std;

//Check to see if a ray hits anything
bool trace(Ray& r, vector<Sphere> spheres, Intersection& i) {
    for(auto s : spheres){
        if(s.intersect(r, i)){
            return true;
        }
    }

    return false;
}

Vec3f cast_ray(Ray& r, vector<Sphere> spheres, vector<Light> lights){
    for(auto s : spheres){
        Intersection intersection;

        if(s.intersect(r, intersection)){
            float diffuse = 0;
            float specular = 0;
            bool path_obstructed = 0;

            for(auto l : lights){
                //Apply lambertian/cosine shading + phong reflection model
                Vec3f point_normal = intersection.normal.normalize();
                Vec3f light_direction = (l.position-intersection.point).normalize();
                
                if(dot(point_normal, light_direction) > 0){
                    float normal_translation_factor = 0.0000001;
                    Ray traced = Ray(intersection.point + (normal_translation_factor*point_normal), light_direction);
                    Intersection shadow_intersection;
                    path_obstructed = trace(traced, spheres, shadow_intersection);

                    if(path_obstructed) {
                        continue;
                    }
                }

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

            return (s.material.colour * (diffuse + s.material.ambient) + Vec3f(1,1,1)*(!path_obstructed*specular));
        }
    } 
    
    return Vec3f(0.298, 0.7058, 0.9843);
    //return Vec3f(0.1, 0.1, 0.1);
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
        
        //normalize large pixel values
        if (max > 1) {
            v = (1/max)*v;
        }
        
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
    vector<Light> lights;
    Material red(Vec3f(0.3, 0.1, 0.1), 0.8, 0.1, 10);
    Material mat(Vec3f(0.4, 0.4, 0.3), 0.1, 0.3, 40);

    spheres.push_back(Sphere(Vec3f(0,5,-15), 4, red));
    spheres.push_back(Sphere(Vec3f(5,0,-30), 1, mat));
    spheres.push_back(Sphere(Vec3f(-5,0,-30), 6, red));
    spheres.push_back(Sphere(Vec3f(-10,5, -30), 4, mat));
    spheres.push_back(Sphere(Vec3f( 8,    5,   -18), 4, red));
    //spheres.push_back(Sphere(Vec3f(-20,10,-35), 3, mat));

     //spheres.push_back(Sphere(Vec3f(-3,    0,   -18), 2,      mat));
     //spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, red));
     //spheres.push_back(Sphere(Vec3f( 1.5, -0.5, -18), 3, red));
     //spheres.push_back(Sphere(Vec3f( 7,    5,   -18), 4,      mat));

    lights.push_back(Light(Vec3f(-20, 20,  20), 1));
    lights.push_back(Light(Vec3f( 30, 50, -25), 1));
    lights.push_back(Light(Vec3f( 30, 20,  30), 1));

    render(framebuffer, height, width, spheres, lights);
    write_image_to_file(framebuffer, height, width);

    return 0;
}