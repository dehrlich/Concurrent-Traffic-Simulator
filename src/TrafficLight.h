#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum TrafficLightPhase
{
    red,
    green,
};


// Definition of class "MessageQueue“ which has the public methods send and receive. 
// Send takes an rvalue reference of type TrafficLightPhase whereas receive returns this type. 
// The class defines an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Defines std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
  T receive();
  void send(T &&msg);
  
  std::deque<TrafficLightPhase> _queue;

private:
  std::mutex _mtx;
  std::condition_variable _condition;
    
};

// Definition of class "TrafficLight“ which is a child class of TrafficObject. 
// The class has public methods "void waitForGreen()“ and "void simulate()“ 
// as well as "TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either "red“ or "green“. Adds the private method "void cycleThroughPhases()“. 
// Defines private member _currentPhase which can take "red“ or "green“ as its value. 

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // Creates private member of type MessageQueue for messages of type TrafficLightPhase 
    // and uses it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _messageQueue;
};

#endif