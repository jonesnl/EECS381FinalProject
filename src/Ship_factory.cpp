#include "Ship_factory.h"

#include "Cruiser.h"
// #include "Tanker.h"

using namespace std;

Ship * create_ship(const string& name,
        const string& type, Point initial_position) {
    if (name == "Cruiser") {
        return new Cruiser(name, initial_position);
    } else if (name == "Tanker") {
        // TODO
    }
}
