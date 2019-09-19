#ifndef STRUCTSH
#define STRUCTSH

#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

struct Material {
    Vec3f colour;

    Material(const Vec3f col){
        colour = col;
    }

    Material() {
        colour = Vec3f(0.9, 0.9, 0.6);
    }
};

struct Light {
    Vec3f position;
    Vec3f colour;
    float intensity;

    Light(const Vec3f& pos, float isty) {
        position = pos;
        intensity = isty;
    }

    Light(const Vec3f& pos, const Vec3f& col, float isty) {
        position = pos;
        colour = col;
        intensity = isty;
    }
};

struct Ray { //Ray represented by equation of line: origin + direction*t
    Vec3f origin;
    Vec3f direction;

    Ray() {}

    Ray(Vec3f& o, Vec3f& d) {
        origin = o;
        direction = d;
    }

    Ray(const Vec3f& o, const Vec3f& d) {
        origin = o;
        direction = d;
    }

    Vec3f point_at_time(float t){
        return origin + direction*t;
    }
};

struct Intersection {
    Vec3f normal;
    float time;
    Ray ray;

    Intersection() {
    }
};

struct Sphere {
    Vec3f center;
    float radius;
    Material material;

    Sphere(const Vec3f& c, float r) {
        center = c;
        radius = r;
    }

    Sphere(const Vec3f& c, float r, const Material& mat) {
        center = c;
        radius = r;
        material = mat;
    }

    Sphere(Vec3f& c, float r) {
        center = c;
        radius = r;
    }

    Sphere() {}

    bool intersect(Ray& r, Intersection& intersection_point){
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
        float discriminant = b*b-4*a*c;

        if(discriminant > 0){
            float t1 = (-b + sqrtf(b*b - 4*a*c))/(2*a);
            float t2 = (-b - sqrtf(b*b - 4*a*c))/(2*a);

            if(t1 > 0){ //point on line(time) = origin + direction*time
                intersection_point.time = t1;
                intersection_point.ray = r;
                intersection_point.normal = (r.point_at_time(t1) - center).normalize();
                return true;
            } else if(t2 > 0){
                intersection_point.time = t2;
                intersection_point.ray = r;
                intersection_point.normal = (r.point_at_time(t2) - center).normalize();
                return true;
            }
        }

        return false;
    }
};

#endif