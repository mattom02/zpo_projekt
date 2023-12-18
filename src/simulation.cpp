#include "simulation.hpp"

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf){
    Time turn = 1;
    while (turn <= d){
        f.do_deliveries(turn);
        f.do_package_passing();
        f.do_work(turn);
        rf(f,turn);
        turn++;
    }
}