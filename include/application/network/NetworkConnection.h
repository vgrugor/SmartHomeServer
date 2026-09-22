#ifndef NETWORK_CONNECTION_H
#define NETWORK_CONNECTION_H

class NetworkConnection {
    public:
        virtual ~NetworkConnection() = default;
        virtual void start() = 0;
        virtual void reconnect() = 0;
        virtual bool isConnected() const = 0;
};

#endif // NETWORK_CONNECTION_H
