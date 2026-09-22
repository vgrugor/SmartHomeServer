#ifndef EVENT_NOTIFIER_H
#define EVENT_NOTIFIER_H

#include <vector>
#include "application/events/EventType.h"
#include "application/events/Observer.h"

class EventNotifier {
    private:
        std::vector<Observer*> observers;
        EventNotifier(const EventNotifier&) = delete;
        EventNotifier& operator=(const EventNotifier&) = delete;

    public:
        EventNotifier();
        static EventNotifier& getInstance();
        bool addObserver(Observer* observer);
        bool removeObserver(Observer* observer);
        void notifyObservers(EventType eventType, const char* message = "");
};

#endif // EVENT_NOTIFIER_H
