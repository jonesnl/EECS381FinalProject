#ifndef UTILITIES_H
#define UTILITIES_H
#include <exception>
#include <memory>
#include <type_traits>

#include "Geometry.h"

class Sim_object;

// Error class used in this project, acts pretty much just like std::exception
class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

// Constant used for some double comparisons if we only care about the value being
// within a margin of error
extern const double double_close_enough_c;

// Constant that says how long a name abbreviation is in the project
extern const size_t name_abbreviation_length_c;

// Allows us to compare how far two objects are from a common point
class DistComp {
public:
    DistComp(Point comparison_point);
    bool operator() (const std::shared_ptr<Sim_object>& i1,
            const std::shared_ptr<Sim_object>& i2);
private:
    Point common_loc;
};

// Compare two Sim_objects based on their names. Also allows you to compare
// a Sim_object's name with a provided string for heterogenous lookup.
struct NameComp {
    // Needed for heterogenous lookup
	using is_transparent = std::true_type;

    // Comparators
    bool operator() (const std::shared_ptr<Sim_object>& p1,
            const std::shared_ptr<Sim_object>& p2);
	bool operator() (const std::string& name1,
			const std::shared_ptr<Sim_object>& p2);
	bool operator() (const std::shared_ptr<Sim_object>& p1,
			const std::string& name2);
};

#endif
