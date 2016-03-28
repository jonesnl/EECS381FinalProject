#ifndef UTILITIES_H
#define UTILITIES_H
#include <exception>
#include <memory>

#include "Geometry.h"

class Island;


class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

/* add any of your own declarations here */
extern const double double_close_enough_c;
extern const int name_abbreviation_length_c;

class IslandDistComp {
public:
    IslandDistComp(Point comparison_point);
    bool operator() (std::shared_ptr<Island> i1, std::shared_ptr<Island> i2);
private:
    Point common_location;
};

struct IslandNameComp {
    bool operator() (const std::shared_ptr<Island>& p1,
            const std::shared_ptr<Island>& p2);
};

std::string name_abrv(const std::string& name);

#endif
