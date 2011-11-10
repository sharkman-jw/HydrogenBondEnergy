#ifndef _ATOM_HPP_
#define _ATOM_HPP_

class Atom
{
public:
    Atom() : x_(0.0), y_(0.0), z_(0.0) {}
    Atom(const Atom& other) : x_(other.x_), y_(other.y_), z_(other.z_) {}
    Atom& operator=(const Atom& rhs)
    {
        if (this != &rhs)
        {
            x_ = rhs.x_;
            y_ = rhs.y_;
            z_ = rhs.z_;
        }
        return *this;
    }
    ~Atom() {}
    
    void set_coordinates(double x, double y, double z)
    { x_ = x; y_ = y; z_ = z; }
    double x() const { return x_; }
    double y() const { return y_; }
    double z() const { return z_; }
        
private:
    double x_;
    double y_;
    double z_;
};

#endif
