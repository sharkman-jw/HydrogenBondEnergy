#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <numeric>
#include "atom.hpp"
#include "snapshot.hpp"

////////////////////////////////////////////////////////////////////////////////
// Config
//
class Config
{
public:
    ~Config();
    static Config* instance()
    {
        if (instance_ == NULL)
        {
            std::cout << "Create Config instance." << std::endl;
            instance_ = new Config;
        }
        return instance_;
    }
    
    void load_config_file(const char* filepath);
    
//private:
    Config() 
        : atoms_(), calc_pairs_(), ss_range_(0, 0), ss_size_(0)
        , scale_factor_(1.0) {}
    Config(const Config& other);
    Config& operator=(const Config& rhs);
    
    bool validate();
    
    void add_atom(const std::string& name, int serial, double charge)
    { atoms_[name] = std::make_pair(serial, charge); }
    
    void add_calc_pair(const std::string& atom1, const std::string& atom2)
    {
        for (std::list<std::pair<std::string, std::string> >::iterator it =
            calc_pairs_.begin(); it != calc_pairs_.end(); ++ it)
        {
            if (it->first == atom1 && it->second == atom2)
                return;
        }
        calc_pairs_.push_back(std::make_pair(atom1, atom2));
    }
    
    static Config* instance_;
    
    std::map<std::string, std::pair<int, double> > atoms_;
    std::list<std::pair<std::string, std::string> > calc_pairs_;
    std::pair<unsigned int, unsigned int> ss_range_; // start and count
    size_t ss_size_;
    double scale_factor_;
};

Config* Config::instance_ = NULL;



bool load_snapshot(std::ifstream& fi, size_t atoms_count, Snapshot* ss);
double calculate_energy(const Snapshot& ss, const Config& config);

int main(int argc, char** argv)
{
    const char* filepath = "test.crd";
    const char* config_filepath = "config.txt";
    
    Config& config = *Config::instance();
    config.add_atom("N1", 1031, -0.415700);
    config.add_atom("H1", 1032, 0.271900);
    config.add_atom("C1", 1084, 0.597300);
    config.add_atom("O1", 1085, -0.567900);
    config.add_calc_pair("N1", "C1");
    config.add_calc_pair("H1", "C1");
    config.add_calc_pair("N1", "O1");
    config.add_calc_pair("H1", "O1");
    config.ss_size_ = 1773;
    config.ss_range_ = std::make_pair(0, 0);
    config.scale_factor_ = 331.5;
    
    std::ifstream fi(filepath);
    if (!fi.is_open())
    {
        std::cout << "File open failed: " << filepath << std::endl;
        return -1;
    }
    
    // read the first dummy line
    std::string line;
    getline(fi, line);
    
    // load snapshots
    //std::list<Snapshot *> ss_list;
    unsigned int n = 0;
    unsigned int ss_start = config.ss_range_.first;
    unsigned int ss_end = config.ss_range_.first + config.ss_range_.second;
    size_t count = config.ss_size_;
    //Snapshot* ss = NULL;
    Snapshot ss;
    std::list<double> energy_list;
    while (true)
    {
        if (n >= ss_start && (n < ss_end || 0 == config.ss_range_.second)) // in range
        {
            //Snapshot* ss = new Snapshot;
            //if (!load_snapshot(fi, count, ss))
            if (!load_snapshot(fi, count, &ss))
                break;
            std::cout << "LOADED Snapshot [" << n << "]" << std::endl;
            //ss_list.push_back(ss);
            
            energy_list.push_back(calculate_energy(ss, config));
        }
        else // out of range
        {
            load_snapshot(fi, count, NULL);
            std::cout << "SKIPPED Snapshot [" << n << "]" << std::endl;
            if (n >= ss_end) // beyond ending point
                break;
        }
        ++ n;
    }
    fi.close();
    
    // calculate average energy
    std::cout << "Average: " <<
        std::accumulate(energy_list.begin(), energy_list.end(), 0.0) 
        / energy_list.size() << std::endl;
    
    return 0;
}

//------------------------------------------------------------------------------
// Load a snapshot from file.
//------------------------------------------------------------------------------
bool load_snapshot(std::ifstream& fi, size_t atoms_count, Snapshot* ss)
{
    double x(0.0), y(0.0), z(0.0);
    Atom* atom = NULL;
    if (ss != NULL)
        ss->clear();
    for (size_t i = 0; i < atoms_count; ++ i)
    {
        fi >> x >> y >> z;
        if (!fi.good())
            return false;
        //std::cout << x << " " << y << " "  << z << " "  << std::endl;
        if (NULL != ss)
        {
            atom = new Atom;
            atom->set_coordinates(x, y, z);
            ss->add(atom);
        }
    }
    return true;
}

double calculate_energy(const Snapshot& ss, const Config& config)
{
    // retrieve atoms
    Atom* atom = NULL;
    std::map<std::string, Atom *> atoms;
    for (std::map<std::string, std::pair<int, double> >::const_iterator it1 =
        config.atoms_.begin(); it1 != config.atoms_.end(); ++ it1)
    {
        atom = ss.get(it1->second.first);
        if (atom)
        {
            atoms[it1->first] = atom;
            atom->set_charge(it1->second.second);
        }
    }
    // calculate
    Atom* other_atom = NULL;
    std::map<std::string, Atom *>::iterator it3;
    double energy = 0.0;
    double total_energy = 0.0;
    double distance = 0.0;
    for (std::list<std::pair<std::string, std::string> >::const_iterator it2 =
        config.calc_pairs_.begin(); config.calc_pairs_.end() != it2; 
        ++ it2)
    {
        it3 = atoms.find(it2->first);
        if (it3 == atoms.end() || it3->second == NULL)
            continue;
        atom = it3->second;
        it3 = atoms.find(it2->second);
        if (it3 == atoms.end() || it3->second == NULL)
            continue;
        other_atom = it3->second;
        
        distance = atom->distance_from(*other_atom);
        
        energy = atom->charge() * other_atom->charge() / distance 
            * config.scale_factor_;
        total_energy += energy;
        
        std::cout << energy << '\t' << total_energy << std::endl;
    }
    return total_energy;
}

