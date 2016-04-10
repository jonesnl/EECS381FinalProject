#ifndef SHIP_COMPOSITE_H
#define SHIP_COMPOSITE_H

#include "Ship_component.h"

#include <map>

class Ship_composite : public Ship_component, public std::enable_shared_from_this<Ship_composite> {
public:
    Ship_composite(const std::string& name_);

    ~Ship_composite() override;

    /*** Component functions ***/
    void add_component(std::shared_ptr<Ship_component> ship_ptr) override;

    void remove_component(const std::string& name) override;

    std::shared_ptr<Ship_component> get_child(const std::string& name) override;

    /*** Readers ***/
    bool can_move() const override;

    bool is_moving() const override;

    bool is_docked() const override;

    bool is_afloat() const override;

    bool can_dock(std::shared_ptr<Island> island_ptr) const override;

    Point get_location() const override;

    std::shared_ptr<Island> get_docked_Island() const override;

    std::shared_ptr<Island> get_destination_Island() const override;

    double get_maximum_speed() const override;

    void update() override;

    void describe() const override;

    void broadcast_current_state() const override;

    /*** Command functions ***/
    void set_destination_position_and_speed(Point destination_position,
            double speed) override;

    void set_destination_island_and_speed(
            std::shared_ptr <Island> destination_island, double speed) override;

    void set_course_and_speed(double course, double speed) override;

    void stop() override;

    void dock(std::shared_ptr<Island> island_ptr) override;

    void refuel() override;

    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;

    /*** Fat interface command functions ***/
    void set_load_destination(std::shared_ptr<Island> island_ptr) override;

    void set_unload_destination(std::shared_ptr<Island> island_ptr) override;

    void attack(std::shared_ptr<Ship> target_ptr) override;

    void stop_attack() override;

    void start_skimming(Point spill_origin_, int spill_size_) override;

private:
    using ChildrenMap_t = std::map<std::string, std::weak_ptr<Ship_component>>;
    ChildrenMap_t children;
    bool children_if_helper(
            const ChildrenMap_t::value_type&, bool (Ship_component::* func_ptr)()) const;
};

#endif