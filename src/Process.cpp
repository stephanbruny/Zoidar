//
// Created by Stephan Bruny on 11.03.23.
//

#include "Process.h"

Process::Process() {}

void Process::addProcess(process_callack proc) {
    this->processes.push_back(proc);
}

void Process::onUpdate(double dt) {
    for (auto &p : this->processes) {
        bool isRunning = p(dt);
        // TODO: Remove process
    }
}

void Process::clear() {
    this->processes.clear();
}