#include "storage_types.hpp"

void PackageQueue::push(Package &&package){
    stockpile_.emplace_back(std::move(package));
}

Package PackageQueue::pop(){
    Package package;
    switch (queueType_){
        case PackageQueueType::FIFO:
            package = std::move(*stockpile_.begin());
            stockpile_.pop_front();
            return package;
        case PackageQueueType::LIFO:
            package = std::move(*stockpile_.end());
            stockpile_.pop_back();
            return package;
        default:
            return package;
    }
}