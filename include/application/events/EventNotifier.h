#ifndef EVENT_NOTIFIER_H
#define EVENT_NOTIFIER_H

#include <algorithm>
#include <vector>
#include "application/events/EventType.h"
#include "application/events/Observer.h"

class EventNotifier {
    private:
        std::vector<Observer*> observers;
        EventNotifier();
        EventNotifier(const EventNotifier&) = delete;
        EventNotifier& operator=(const EventNotifier&) = delete;

    public:
        static EventNotifier& getInstance();
        void addObserver(Observer* observer);
        void removeObserver(Observer* observer);
        void notifyObservers(EventType eventType, const String& message = "");
};

#endif // EVENT_NOTIFIER_H
