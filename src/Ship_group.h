#ifndef SHIP_COMPOSITE_H
#define SHIP_COMPOSITE_H

#include "Ship_component.h"
#include "Utility.h"

#include <map>
#include <iostream>

/* Ship_group class
 *
 * A Ship_group is a collection of other Ship_groups and Ships.
 *
 * A Ship_group has the same interface as a normal ship, with most operations
 * implemented such that the operation is called on each child object. Notable
 * exceptions include reader functions since they are not needed for simulation
 * purposes at the moment, and receive_hit(), as a group of unrelated ships
 * should not receive a single hit based on current abstractions.
 *
 * To use, add children to the group using add_child(), remove them with remove_child(),
 * clear all children using remove_all_children(), and get children using get_child().
 *
 * It is the group's job to set a child's parent pointer when they are added to the
 * group and reset it when the child is removed from the group. Children should
 * ask their parents to remove them from their groups if necessary, and the parent
 * will clear the child's parent pointer.
 */

class Ship_group : public Ship_component {
public:
    // Name of the Ship_group
    Ship_group(const std::string& name_);

    /*** Component functions ***/
    // Add a child to the group, the child may throw an error if it is already
    // a member of a group. This function throws an error if adding the component
    // to the group would cause a cycle of groups to occur.
    void add_child(std::shared_ptr<Ship_component> ship_ptr) override;

    // Remove a child from the group. Throws an error if the child is not in the group.
    void remove_child(std::shared_ptr<Ship_component> child_ptr) override;

    // Removes all children from the group.
    void remove_all_children() override;

    // Get a child from the group of children by name.
    std::shared_ptr<Ship_component> get_child(const std::string& name) override;

    /*** Ship Readers (Throws exceptions when called) ***/
    bool can_move() const override;
    bool is_moving() const override;
    bool is_docked() const override;
    bool is_afloat() const override;
    bool can_dock(std::shared_ptr<Island> island_ptr) const override;
    Point get_location() const override;
    std::shared_ptr<Island> get_docked_Island() const override;
    std::shared_ptr<Island> get_destination_Island() const override;
    double get_maximum_speed() const override;

    /*** Simulation and Model functions ***/
    // Update the state of the group (currently does nothing)
    void update() override;

    // Describe the group by printing its name and the names of its direct children
    void describe() const override;

    // Broadcast the state of the group (currently does nothing)
    void broadcast_current_state() const override;

    /*** Command functions ***/
    // Each of these functions calls the corresponding function on all children.
    // We handle Errors in these functions so all ships in the group will get
    // the operation applied to them.
    void set_destination_position_and_speed(Point destination_position,
            double speed) override;
    void set_destination_island_and_speed(
            std::shared_ptr <Island> destination_island, double speed) override;
    void set_course_and_speed(double course, double speed) override;
    void stop() override;
    void dock(std::shared_ptr<Island> island_ptr) override;
    void refuel() override;
    // EXCEPTION: receive_hit throws an error if called since a group of unrelated
    // ships should not be able to take a hit all at once
    void receive_hit(int hit_force, std::shared_ptr<Ship_component> attacker_ptr) override;
    void set_load_destination(std::shared_ptr<Island> island_ptr) override;
    void set_unload_destination(std::shared_ptr<Island> island_ptr) override;
    void attack(std::shared_ptr<Ship_component> target_ptr) override;
    void stop_attack() override;
    void start_skimming(Point spill_origin_, int spill_size_) override;

private:
    // Map of the children of the group with the key as the name of the child
    using ChildrenMap_t = std::map<std::string, std::weak_ptr<Ship_component>>;
    ChildrenMap_t children;

    // Run member function func on every child in the children map.
    //
    // func should be a bound member function pointer using mem_fn(func_ptr), or
    // a bind() equivalent that also binds arguments to the function object.
    // func must be able to be called using func(shared_ptr<> child)
    template <typename T>
    void for_each_child_catch(T func) const;

    // See if a child group with the provided name is a member of either this group or
    // any subgroups. This us used to ensure we don't have group cycles.
    bool is_child_member(const std::string& name) const;
};

#endif