#include "Ship_factory.h"

#include "Cruiser.h"
#include "Tanker.h"
#include "Utility.h"

using namespace std;

Ship * create_ship(const string& name,
        const string& type, Point initial_position) {
    if (type == "Cruiser") {
        return new Cruiser(name, initial_position);
    } else if (type == "Tanker") {
        return new Tanker(name, initial_position);
    } else {
        throw Error("Trying to create ship of unknown type!");
    }
}
