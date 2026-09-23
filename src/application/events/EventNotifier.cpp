#include "application/events/EventNotifier.h"
#include <algorithm>

EventNotifier::EventNotifier() {}

EventNotifier& EventNotifier::getInstance() {
    static EventNotifier instance;
    return instance;
}

bool EventNotifier::addObserver(Observer* observer) {
    if (
        observer == nullptr
        || std::find(observers.begin(), observers.end(), observer) != observers.end()
    ) {
        return false;
    }

    observers.push_back(observer);
    return true;
}

bool EventNotifier::removeObserver(Observer* observer) {
    const auto position = std::find(observers.begin(), observers.end(), observer);
    if (position == observers.end()) {
        return false;
    }

    observers.erase(position);
    return true;
}

void EventNotifier::notifyObservers(EventType eventType, const char* message) {
    const char* safeMessage = message == nullptr ? "" : message;

    for (auto observer : observers) {
        observer->update(eventType, safeMessage);
    }
}
