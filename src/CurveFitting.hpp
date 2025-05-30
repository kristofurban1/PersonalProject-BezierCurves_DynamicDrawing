#pragma once

#include <vector>

struct Point
{
    const float x, y;
    Point(float x, float y) : x(x), y(y) {};

    void DebugDisplay(const char* name) const;

    const float len() const;

    const Point operator*(float a) const{
        return Point(x * a, y * a);
    }
    const Point operator+(const Point a) const{
        return Point(x + a.x, y + a.y);
    }
    const Point operator-(const Point a) const{
        return Point(x - a.x, y - a.y);
    }

    static const Point pow(const Point p, int power);
    
};

struct Bezier
{
    const Point P0, P1, P2, P3;
    Bezier(Point p0, Point p1, Point p2, Point p3) : P0(p0), P1(p1), P2(p2), P3(p3) {}

};

const Bezier FitCubicBezier(const std::vector<Point> points);
double EvaluateBezier(const Bezier bezier, const std::vector<Point> points);