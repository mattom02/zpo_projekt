#ifndef NETSIM_STORAGE_TYPES_HPP
#define NETSIM_STORAGE_TYPES_HPP

#include <list>
#include "package.hpp"

enum PackageQueueType{
    FIFO, LIFO
};

class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package &&package) = 0;

    virtual const_iterator begin() const = 0;

    virtual const_iterator cbegin() const = 0;

    virtual const_iterator end() const = 0;

    virtual const_iterator cend() const = 0;

    virtual std::size_t size() const = 0;

    virtual bool empty() const = 0;

    virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile{
public:
    virtual Package pop() = 0;

    virtual PackageQueueType get_queue_type() const = 0;

    ~IPackageQueue() override = default;
};

class PackageQueue : public IPackageQueue{
public:
    PackageQueue(PackageQueueType queueType) : queueType_(queueType) {}

    void push(Package &&package) override;

    Package pop() override;

    PackageQueueType get_queue_type() const override { return queueType_;}

    bool empty() const override { return stockpile_.empty();}

    std::size_t size() const override { return stockpile_.size();}

    const_iterator begin() const override { return stockpile_.begin();}

    const_iterator cbegin() const override { return stockpile_.begin();}

    const_iterator end() const override { return stockpile_.end();}

    const_iterator cend() const override { return stockpile_.end();}

    ~PackageQueue() override = default;
private:
    std::list<Package> stockpile_;
    PackageQueueType queueType_;
};

#endif 

