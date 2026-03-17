#include "LoggingObserver.h"

#include <algorithm>
#include <fstream>

Subject::Subject() {
    Attach(LogObserver::getInstance());
}

Subject::Subject(const Subject&) {
    Attach(LogObserver::getInstance());
}

Subject& Subject::operator=(const Subject&) {
    observers.clear();
    Attach(LogObserver::getInstance());
    return *this;
}

Subject::~Subject() = default;

void Subject::Attach(Observer* observer) {
    if (observer == nullptr) return;
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void Subject::Detach(Observer* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::Notify(const ILoggable* loggable) const {
    for (Observer* observer : observers) {
        if (observer != nullptr) {
            observer->Update(loggable);
        }
    }
}

LogObserver* LogObserver::getInstance() {
    static LogObserver instance;
    return &instance;
}

void LogObserver::Update(const ILoggable* loggable) {
    if (loggable == nullptr) return;

    std::ofstream outFile("gamelog.txt", std::ios::app);
    if (!outFile.is_open()) return;

    outFile << loggable->stringToLog() << std::endl;
}
