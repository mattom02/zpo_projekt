#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP

#include "types.hpp"
#include <set>

class Package{
public:
    Package();

    explicit Package(ElementID id) : id_(id) {assigned_IDs.insert(id);}

    Package(Package&& package) noexcept : id_{package.id_} {}

    Package& operator=(Package &&package) noexcept;

    ElementID get_id() const { return id_;}

    ~Package();

private:
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;
    ElementID id_;
};

#endif 
