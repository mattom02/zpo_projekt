#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "types.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include <map>
#include <optional>
#include <memory>
#include "helpers.hpp"

class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

    using const_iterator = IPackageStockpile::const_iterator;

    virtual const_iterator begin() const = 0;

    virtual const_iterator cbegin() const = 0;

    virtual const_iterator end() const = 0;

    virtual const_iterator cend() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences{
public:
    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : probability_(std::move(pg)) {}

    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    void add_receiver(IPackageReceiver *r);

    void remove_receiver(IPackageReceiver *r);

    IPackageReceiver* choose_receiver();

    const_iterator begin() const { return preferences_.begin();}

    const_iterator cbegin() const { return preferences_.begin();}

    const_iterator end() const { return preferences_.end();}

    const_iterator cend() const { return preferences_.end();}

    const preferences_t& get_preferences() const { return preferences_; }

    ~ReceiverPreferences() = default;
private:
    preferences_t preferences_;
    ProbabilityGenerator probability_;
};

class PackageSender{
public:
    ReceiverPreferences receiver_preferences_;

    PackageSender() = default;

    PackageSender(PackageSender&& packageSender) = default;

    void send_package();

    const std::optional<Package>& get_sending_buffer() const { return buffer_; }

    void push_package(Package&& package);

    ~PackageSender() = default;
private:
    std::optional<Package> buffer_ = std::nullopt;
};

class Ramp : public PackageSender{
public:
    Ramp(ElementID id, TimeOffset di) : PackageSender(), id_(id), di_(di) {}

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const { return di_; }

    ElementID get_id() const { return id_; }

    ~Ramp() = default;
private:
    ElementID id_;
    TimeOffset di_;
    std::optional<Package> buffer_ = std::nullopt;
};

class Worker : public PackageSender, public IPackageReceiver{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    ElementID get_id() const override { return id_; }

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; }

    Time get_package_processing_start_time() const { return st_; }

    void receive_package(Package&& p) override;

    const_iterator begin() const override { return q_->begin(); }

    const_iterator cbegin() const override { return q_->begin(); }

    const_iterator end() const override { return q_->end(); }

    const_iterator cend() const override { return q_->end(); }

    ~Worker() = default;
private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    Time st_;
    std::optional<Package> buffer_ = std::nullopt;
};

class Storehouse : public IPackageReceiver{
public:
    explicit Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move(d)) {}

    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; }

    const_iterator begin() const override { return d_->begin(); }

    const_iterator cbegin() const override { return d_->begin(); }

    const_iterator end() const override { return d_->end(); }

    const_iterator cend() const override { return d_->end(); }

    ~Storehouse() = default;
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

#endif
