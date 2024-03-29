#ifndef SHIP_FACTORY_H
#define SHIP_FACTORY_H

#include "Geometry.h"

#include <string>
#include <memory>

class Ship_component;
/* This is a very simple form of factory, a function; you supply the information, it creates
the specified kind of object and returns a pointer to it. The Ship is allocated
with new, so some other component is responsible for deleting it.
*/

// Construct a new ship of type `type`.
// May throw Error("Trying to create ship of unknown type!")
std::shared_ptr<Ship_component> create_ship(const std::string& name,
        const std::string& type, Point initial_position);

std::shared_ptr<Ship_component> create_group(const std::string& name);

#endif
