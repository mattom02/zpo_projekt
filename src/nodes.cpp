#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver *r){
    if(preferences_.empty()){
        preferences_.emplace(r, 1);
    }
    else{
        preferences_.emplace(r, 0);
        std::size_t prefSize = preferences_.size();
        for(auto& i : preferences_){
            i.second = (1.0/double(prefSize));
        }
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *r){
    preferences_.erase(r);
    if(!preferences_.empty()) {
        std::size_t prefSize = preferences_.size();
        for (auto &i: preferences_) {
            i.second = (1.0 / double(prefSize));
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver(){
    IPackageReceiver* receiver;
    double probability = probability_();
    double distribution = 0.0;
    for(auto& i : preferences_){
        distribution += i.second;
        if (probability < distribution){
            receiver = i.first;
            break;
        }
    }
    return receiver;
}

void PackageSender::push_package(Package&& package){
    if (!buffer_.has_value()) {
        buffer_.emplace(std::move(package));
    }
}

void PackageSender::send_package(){
    if (buffer_.has_value()) {
        receiver_preferences_.choose_receiver()->receive_package(std::move(*buffer_));
        buffer_.reset();
    }
}

void Ramp::deliver_goods(Time t){
    if (t % di_ == 1){
        Package package = Package();
        push_package(std::move(package));
    }
}

void Worker::do_work(Time t){
    if(!buffer_.has_value()){
        buffer_.emplace(q_->pop());
        st_ = t;
    }
    else if(t - st_ + 1 == pd_){
        push_package(std::move(*buffer_));
        buffer_.reset();
    }
}

void Worker::receive_package(Package&& p){
    q_->push(std::move(p));
}

void Storehouse::receive_package(Package&& p){
    d_->push(std::move(p));
}