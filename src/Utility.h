#ifndef UTILITIES_H
#define UTILITIES_H
#include <exception>
#include <memory>

#include "Geometry.h"

class Island;

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
// within a margin of error caused by floating point arithmetic.
extern const double double_close_enough_c;

// Constant that says how long a name abreviation is in the project
extern const size_t name_abbreviation_length_c;

// Allows us to compare how far two islands are from a common point
class IslandDistComp {
public:
    IslandDistComp(Point comparison_point);
    bool operator() (std::shared_ptr<Island> i1, std::shared_ptr<Island> i2);
private:
    Point common_location;
};

// Compare two islands based on their names
struct IslandNameComp {
    bool operator() (const std::shared_ptr<Island>& p1,
            const std::shared_ptr<Island>& p2);
};

#endif
