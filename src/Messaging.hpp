//
// Created by Stephan Bruny on 28.03.23.
//

#ifndef ZOIDAR_MESSAGING_HPP
#define ZOIDAR_MESSAGING_HPP

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace Messaging {

    template<class T>
    struct Message {
        string topic;
        T sender;
    };

    struct Actor {
        string id;
        Actor(string id) : id(id) {};
        virtual void onMessage(Message<shared_ptr<Actor>> message) {}
    };

    template<class TActor = Actor>
    class Messagebus {
    private:
        vector<Message<shared_ptr<TActor>>> queue;
        map<string, vector<shared_ptr<TActor>>> subscriptions;
    public:
        Messagebus(): queue() {};

        void subscribe(string topic, shared_ptr<TActor> actor) {
            auto match = subscriptions.find(topic);
            if (match == subscriptions.end()) {
                subscriptions.insert(pair<string, vector<shared_ptr<TActor>>>(topic, { actor }));
                return;
            }
            auto list = match->second;
            list.push_back(actor);
        }

        void publish(string topic, shared_ptr<TActor> sender) {
            queue.push_back({
                topic, sender
            });
        }

        void flush() {
            for (auto &message : queue) {
                auto matches = subscriptions.find(message.topic);
                if (matches != subscriptions.end()) {
                    auto subs = matches->second;
                    for (auto &sub : subs) {
                        sub->onMessage(message);
                    }
                }
            }
            queue.clear();
        }
    };
}

#endif //ZOIDAR_MESSAGING_HPP
