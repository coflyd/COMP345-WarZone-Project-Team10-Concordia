#ifndef LOGGINGOBSERVER_H
#define LOGGINGOBSERVER_H

#include <string>
#include <vector>

class ILoggable {
public:
    virtual std::string stringToLog() const = 0;
    virtual ~ILoggable() = default;
};

class Observer {
public:
    virtual void Update(const ILoggable* loggable) = 0;
    virtual ~Observer() = default;
};

class Subject {
private:
    std::vector<Observer*> observers;

public:
    Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject();

    void Attach(Observer* observer);
    void Detach(Observer* observer);

protected:
    void Notify(const ILoggable* loggable) const;
};

class LogObserver : public Observer {
private:
    LogObserver() = default;

public:
    static LogObserver* getInstance();
    void Update(const ILoggable* loggable) override;
};

#endif
