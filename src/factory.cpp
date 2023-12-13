#include "factory.hpp"
#include <stdexcept>
#include <sstream>

void Factory::do_deliveries(Time time){
    for (auto i : ramps_){
        i.deliver_goods(time);
    }
}

void Factory::do_package_passing(){
    for (auto i : ramps_){
        i.send_package();
    }
    for (auto i : workers_){
        i.send_package();
    }
}

void Factory::do_work(Time time){
    for (auto i : workers_){
        i.do_work(time);
    }
}

bool Factory::has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if(node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;
    if (sender->receiver_preferences_.get_preferences().empty()){
        throw std::logic_error("No receivers were defined.");
    }
    bool hasOtherReceiver = false;
    for (auto i : sender->receiver_preferences_.get_preferences()){
        if (i.first->get_receiver_type() == STOREHOUSE){
            hasOtherReceiver = true;
        }
        else if(i.first->get_receiver_type() == WORKER){
            IPackageReceiver* receiver_ptr = i.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if(sendrecv_ptr == sender){
                continue;
            }
            hasOtherReceiver = true;
            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;
    if(hasOtherReceiver){
        return true;
    }
    else{
        throw std::logic_error("There is no reachable storehouse.");
    }
}


bool Factory::is_consistent(){
    std::map<const PackageSender*, NodeColor> node_colors;
    for(auto i : ramps_){
        node_colors[&i] = NodeColor::UNVISITED;
    }
    for(auto i : workers_){
        node_colors[&i] = NodeColor::UNVISITED;
    }
    try{
        for (auto i : ramps_){
            has_reachable_storehouse(&i, node_colors);
        }
        return true;
    }
    catch(std::logic_error &e){
        return false;
    }
}

void Factory::remove_receiver(NodeCollection<Worker>& collection, ElementID id) {
    Worker& receiver = *collection.find_by_id(id);
    for (auto &i : ramps_){
        i.receiver_preferences_.remove_receiver(&receiver);
    }
}

void Factory::remove_receiver(NodeCollection<Storehouse>& collection, ElementID id) {
    Storehouse& receiver = *collection.find_by_id(id);
    for (auto &i : workers_){
        i.receiver_preferences_.remove_receiver(&receiver);
    }
}

void Factory::remove_storehouse(ElementID id){
    remove_receiver(storehouses_, id);
    storehouses_.remove_by_id(id);
}

void Factory::remove_worker(ElementID id){
    remove_receiver(workers_, id);
    workers_.remove_by_id(id);
}

ParsedLineData parse_line(std::string line){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream string_stream(line);
    std::vector<std::string> parametersKV;

    while (std::getline(string_stream, token, ' ')){
        tokens.push_back(token);
    }
    for (auto i : tokens){
        std::istringstream parameters(i);
        while (std::getline(parameters, token, '=')){
            parametersKV.push_back(token);
        }
    }
    std::map<std::string, std::string> map;
    for(std::size_t i = 1; i < parametersKV.size(); i += 2){
        map.emplace(parametersKV[i], parametersKV[i+1]);
    }
    if (parametersKV[0] == "LOADING_RAMP"){
        return ParsedLineData(ElementType::RAMP, std::move(map));
    }
    else if (parametersKV[0] == "WORKER"){
        return ParsedLineData(ElementType::WORKER, std::move(map));
    }
    else if (parametersKV[0] == "STOREHOUSE"){
        return ParsedLineData(ElementType::STOREHOUSE, std::move(map));
    }
    else if (parametersKV[0] == "LINK"){
        return ParsedLineData(ElementType::LINK, std::move(map));
    }
    else throw std::logic_error("Wrong element type.");
}

Factory load_factory_structure(std::istream& is){
    Factory factory;
    std::string line;
    ParsedLineData parsedLineData;

    while(std::getline(is, line)){
        if(line.empty() or line.find(';') != std::string::npos){
            continue;
        }
        parsedLineData = parse_line(line);
        auto parameters = parsedLineData.get_parameters();
        if(parsedLineData.get_element_type() == ElementType::RAMP){
            Ramp ramp = Ramp(std::stoull(parameters["id"]), std::stoull(parameters["delivery-interval"]));
            factory.add_ramp(std::move(ramp));
        }
        else if(parsedLineData.get_element_type() == ElementType::WORKER){
            if(parameters["queue-type"] == "FIFO"){
                Worker worker = Worker(std::stoull(parameters["id"]), std::stoull(parameters["processing-time"]), std::make_unique<PackageQueue>(PackageQueueType::FIFO));
                factory.add_worker(std::move(worker));
            }
            else if (parameters["queue-type"] == "LIFO"){
                Worker worker = Worker(std::stoull(parameters["id"]), std::stoull(parameters["processing-time"]), std::make_unique<PackageQueue>(PackageQueueType::LIFO));
                factory.add_worker(std::move(worker));
            }
        }
        else if(parsedLineData.get_element_type() == ElementType::STOREHOUSE){
            Storehouse storehouse = Storehouse(std::stoull(parameters["id"]));
            factory.add_storehouse(std::move(storehouse));
        }
        else if(parsedLineData.get_element_type() == ElementType::LINK){
            std::string strSrcID;
            std::string strDestID;
            for (auto i : parameters["src"]) {
                if (std::isdigit(i)) {
                    strSrcID += i;
                }
            }
            for (auto i : parameters["dest"]) {
                if (std::isdigit(i)) {
                    strDestID += i;
                }
            }
            ElementID srcID = std::stoull(strSrcID);
            ElementID destID = std::stoull(strDestID);
            if(parameters["src"].find("ramp") != std::string::npos){
                if(parameters["dest"].find("worker") != std::string::npos){
                    Worker& worker = *factory.find_worker_by_id(destID);
                    factory.find_ramp_by_id(srcID)->receiver_preferences_.add_receiver(&worker);
                }
                else if(parameters["dest"].find("store") != std::string::npos){
                    Storehouse& storehouse = *factory.find_storehouse_by_id(destID);
                    factory.find_ramp_by_id(srcID)->receiver_preferences_.add_receiver(&storehouse);
                }
            }
            else if(parameters["src"].find("worker") != std::string::npos){
                if(parameters["dest"].find("worker") != std::string::npos){
                    Worker& worker = *factory.find_worker_by_id(destID);
                    factory.find_worker_by_id(srcID)->receiver_preferences_.add_receiver(&worker);
                }
                else if(parameters["dest"].find("store") != std::string::npos){
                    Storehouse& storehouse = *factory.find_storehouse_by_id(destID);
                    factory.find_worker_by_id(srcID)->receiver_preferences_.add_receiver(&storehouse);
                }
            }
        }
    }
    return factory;
}

void save_factory_structure(Factory& factory, std::ostream& os){
    os << "; == LOADING RAMPS ==\n\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&os](Ramp ramp) {os << "LOADING_RAMP id=" << ramp.get_id() << " delivery-interval=" << ramp.get_delivery_interval() << "\n";});
    os << "\n; == WORKERS ==\n\n";
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&os](Worker worker) {os << "WORKER id=" << worker.get_id() << " processing-time=" << worker.get_processing_duration() << " queue-type=" << (worker.get_queue()->get_queue_type() == PackageQueueType::FIFO ? "FIFO" : "LIFO") << "\n";});
    os << "\n; == STOREHOUSES ==\n\n";
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), [&os](Storehouse storehouse) {os << "STOREHOUSE id=" << storehouse.get_id() << "\n";});
    os << "\n; == LINKS ==\n\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&os](Ramp ramp) {
        for (auto i : ramp.receiver_preferences_.get_preferences()){
            if(i.first->get_receiver_type() == ReceiverType::WORKER){
                os << "LINK src=ramp-" << ramp.get_id() << " dest=worker-" << i.first->get_id() << "\n";
            }
            if(i.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                os << "LINK src=ramp-" << ramp.get_id() << " dest=store-" << i.first->get_id() << "\n";
            }
        }
        os << "\n";
    });
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&os](Worker worker) {
        for (auto i : worker.receiver_preferences_.get_preferences()){
            if(i.first->get_receiver_type() == ReceiverType::WORKER){
                os << "LINK src=worker-" << worker.get_id() << " dest=worker-" << i.first->get_id() << "\n";
            }
            if(i.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                os << "LINK src=worker-" << worker.get_id() << " dest=store-" << i.first->get_id() << "\n";
            }
        }
        os << "\n";
    });
}
