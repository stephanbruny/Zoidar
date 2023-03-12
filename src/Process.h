//
// Created by Stephan Bruny on 11.03.23.
//

#ifndef ZOIDAR_PROCESS_H
#define ZOIDAR_PROCESS_H

#include <functional>
#include <vector>

using process_callack = std::function<bool(double)>;

class Process {
public:
    Process();
    void onUpdate(double dt);
    void addProcess(process_callack);
    void clear();
private:
    std::vector<process_callack> processes;
};


#endif //ZOIDAR_PROCESS_H
