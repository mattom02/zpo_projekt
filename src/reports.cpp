#include "reports.hpp"
#include "sstream"

void generate_structure_report(const Factory& f, std::ostream& os){
    os << "\n== LOADING RAMPS ==\n\n";
    std::for_each(f.ramp_cbegin(), f.ramp_cend(), [&os](const Ramp& ramp) {
        os << "LOADING RAMP #" << ramp.get_id() <<"\n  Delivery interval: " << ramp.get_delivery_interval() << "\n  Receivers:";
        for (const auto& i : ramp.receiver_preferences_.get_preferences()){
            if(i.first->get_receiver_type() == ReceiverType::WORKER){
                os <<"\n    worker #" << i.first->get_id();
            }
            else if(i.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                os <<"\n    storehouse #" << i.first->get_id();
            }
        }
        os << "\n\n";
    });
    os << "\n== WORKERS ==\n\n";
    std::for_each(f.worker_cbegin(), f.worker_cend(), [&os](const Worker& worker) {
        os << "WORKER #" << worker.get_id() << "\n  Processing time: " << worker.get_processing_duration() << "\n  Queue type: " << (worker.get_queue()->get_queue_type() == PackageQueueType::FIFO ? "FIFO" : "LIFO") << "\n  Receivers:";
        for (const auto& i : worker.receiver_preferences_.get_preferences()){
            if(i.first->get_receiver_type() == ReceiverType::WORKER){
                os <<"\n    worker #" << i.first->get_id();
            }
            else if(i.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                os <<"\n    storehouse #" << i.first->get_id();
            }
        }
        os << "\n\n";
    });
    os << "\n== STOREHOUSES ==\n\n";
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), [&os](const Storehouse& storehouse) {os << "STOREHOUSE #" << storehouse.get_id() << "\n\n";});
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t){
    os << "=== [ Turn: " << t << " ] ===\n\n== WORKERS ==\n\n";
    std::for_each(f.worker_cbegin(), f.worker_cend(), [&os, t](const Worker& worker) {
        os << "WORKER #" << worker.get_id() << "\n  PBuffer: ";
        if(worker.get_processing_buffer().has_value()) {
            os << "#" << worker.get_processing_buffer()->get_id() << " (pt = " << worker.get_package_processing_start_time() - t + 1 << ")\n";
        }
        else{
            os << "(empty)\n";
        }
        os << "  Queue:";
        if(worker.get_queue()->empty()){
            os << " (empty)";
        }
        else {
            for (const auto &i: *worker.get_queue()) {
                if (i.get_id() != worker.get_queue()->cbegin()->get_id()) {
                    os << ", #" << i.get_id();
                } else {
                    os << " #" << i.get_id();
                }
            }
        }
        os << "\n  SBuffer: ";
        if(worker.get_sending_buffer().has_value()){
            os << "#" << worker.get_sending_buffer()->get_id();
        }
        else{
            os << "(empty)";
        }
        os << "\n\n";
    });
    os << "\n== STOREHOUSES ==\n\n";
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), [&os](const Storehouse& storehouse) {
        os << "STOREHOUSE #" << storehouse.get_id() << "\n  Stock:";
        if(storehouse.get_stockpile()->empty()){
            os << " (empty)";
        }
        else {
            for (const auto &i: *storehouse.get_stockpile()) {
                if (i.get_id() != storehouse.get_stockpile()->cbegin()->get_id()) {
                    os << ", #" << i.get_id();
                } else {
                    os << " #" << i.get_id();
                }
            }
        }
    });
    os << "\n\n";
}