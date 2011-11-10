#ifndef _SNAPSHOT_HPP_
#define _SNAPSHOT_HPP_

#include <vector>
#include <cstdio>
#include "atom.hpp"

class Snapshot
{
public:
    Snapshot() : atoms_() {}
    ~Snapshot() { clear(); }
    
    void clear()
    {
        for (std::vector<Atom *>::iterator it = atoms_.begin();
            it != atoms_.end(); ++it)
            delete *it;
        atoms_.clear();
    }
    
    void add(Atom* atom)
    {
        if (NULL != atom)
            atoms_.push_back(atom);
    }
    
    Atom* get(unsigned int serial)
    {
        return (serial > 0 && serial <= atoms_.size()) ? atoms_[serial - 1] : NULL;
    }
    
    void print(unsigned int n = 0)
    {
        if (0 == n)
        {
            n = 1;
            for (std::vector<Atom *>::iterator it = atoms_.begin();
                it != atoms_.end(); ++it)
            {
                printf("[%u]%8.3f%8.3f%8.3f\n",
                    n++, (*it)->x(), (*it)->y(), (*it)->z());
            }
            return;
        }
        Atom* atom = get(n);
        if (NULL == atom)
            printf("Invalid atom serial number: %u", n);
        else
            printf("[%u]%8.3f%8.3f%8.3f\n", n, atom->x(), atom->y(), atom->z());
    }
    
private:
    Snapshot(const Snapshot& other);
    Snapshot& operator=(Snapshot rhs);
    
    std::vector<Atom *> atoms_;
};

#endif
