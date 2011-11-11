#ifndef _ATOM_HPP_
#define _ATOM_HPP_

#include <cmath>

class Atom
{
public:
    Atom() : x_(0.0), y_(0.0), z_(0.0), charge_(0.0) {}
    Atom(const Atom& other)
        : x_(other.x_), y_(other.y_), z_(other.z_), charge_(other.charge_) {}
    Atom& operator=(const Atom& rhs)
    {
        if (this != &rhs)
        {
            x_ = rhs.x_;
            y_ = rhs.y_;
            z_ = rhs.z_;
            charge_ = rhs.charge_;
        }
        return *this;
    }
    ~Atom() {}
    
    double distance_from(const Atom& other) const
    {
        double dx = x_ - other.x_;
        double dy = y_ - other.y_;
        double dz = z_ - other.z_;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    void set_coordinates(double x, double y, double z)
    { x_ = x; y_ = y; z_ = z; }
    void set_charge(double charge) { charge_ = charge; }
    
    double x() const { return x_; }
    double y() const { return y_; }
    double z() const { return z_; }
    double charge() const { return charge_; }
        
private:
    double x_;
    double y_;
    double z_;
    double charge_;
};

#endif
