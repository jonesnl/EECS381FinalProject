#include "Ship_factory.h"

#include "Cruiser.h"
#include "Torpedo_boat.h"
#include "Tanker.h"
#include "Cruise_ship.h"
#include "Skimmer.h"
#include "Ship_group.h"
#include "Utility.h"

using namespace std;

// Create a new ship of type `type`. Return a pointer to the new ship.
// If the type is unrecognized, throw an error.
shared_ptr<Ship_component> create_ship(const string& name,
        const string& type, Point initial_position) {
    if (type == "Cruiser") {
        return make_shared<Cruiser>(name, initial_position);
    } else if (type == "Tanker") {
        return make_shared<Tanker>(name, initial_position);
    } else if (type == "Cruise_ship") {
        return make_shared<Cruise_ship>(name, initial_position);
    } else if (type == "Torpedo_boat") {
        return make_shared<Torpedo_boat>(name, initial_position);
    } else if (type == "Skimmer") {
        return make_shared<Skimmer>(name, initial_position);
    } else if (type == "Group") {
        return make_shared<Ship_group>(name);
    } else {
        throw Error("Trying to create ship of unknown type!");
    }
}

shared_ptr<Ship_component> create_group(const string& name) {
    return make_shared<Ship_group>(name);
}