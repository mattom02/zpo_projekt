#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include "nodes.hpp"
#include <vector>

template <class Node> class NodeCollection{
public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) { collection_.emplace_back(std::move(node)); }

    iterator begin() { return collection_.begin(); }

    iterator end() { return collection_.end(); }

    const_iterator begin() const { return collection_.begin(); }

    const_iterator end() const { return collection_.end(); }

    const_iterator cbegin() const { return collection_.begin(); }

    const_iterator cend() const { return collection_.end(); }

    iterator find_by_id(ElementID id) { return std::find_if(begin(), end(), [id](auto node){ return node.get_id() == id; } ); }

    const_iterator find_by_id(ElementID id) const { return std::find_if(cbegin(), cend(), [id](Node node){ return node.get_id() == id; }); }

    void remove_by_id(ElementID id) { auto foundID = find_by_id(id); if(foundID != end()) collection_.erase(foundID); }
private:
    container_t collection_;
};

class Factory{
public:
    Factory() = default;

    void add_ramp(Ramp&& ramp) { ramps_.add(std::move(ramp)); }

    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }

    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); }

    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramps_.cend(); }

    void add_storehouse(Storehouse&& storehouse) { storehouses_.add(std::move(storehouse)); }

    void remove_storehouse(ElementID id);

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }

    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); }

    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return storehouses_.cend(); }

    void add_worker(Worker&& worker) { workers_.add(std::move(worker)); }

    void remove_worker(ElementID id);

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }

    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }

    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin(); }

    NodeCollection<Worker>::const_iterator worker_cend() const { return workers_.cend(); }

    void do_deliveries(Time time);

    void do_package_passing();

    void do_work(Time time);

    enum class NodeColor { UNVISITED, VISITED, VERIFIED };

    bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);

    bool is_consistent();

    void remove_receiver(NodeCollection<Worker>& collection, ElementID id);

    void remove_receiver(NodeCollection<Storehouse>& collection, ElementID id);
private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Storehouse> storehouses_;
    NodeCollection<Worker> workers_;
};

enum class ElementType{
    RAMP, WORKER, STOREHOUSE, LINK
};

class ParsedLineData{
public:
    ParsedLineData() = default;

    ParsedLineData(ElementType elementType, std::map<std::string, std::string> &&parameters) : element_type_(elementType), parameters_(parameters) {}

    ElementType get_element_type() const { return element_type_; }

    std::map<std::string, std::string> get_parameters() const { return parameters_; }

    ~ParsedLineData() = default;
private:
    ElementType element_type_;
    std::map<std::string,std::string> parameters_;
};

ParsedLineData parse_line(std::string line);

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostream& os);

#endif
