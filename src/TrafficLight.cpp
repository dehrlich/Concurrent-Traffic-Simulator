#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // The method receive uses std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object is returned by the receive function. 
    std::unique_lock<std::mutex> lck(_mtx);
    _condition.wait(lck, [this] {return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();
    lck.unlock();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // The method send uses the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(_mtx);
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{
}

void TrafficLight::waitForGreen()
{
    // Implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true) {
      TrafficLightPhase phase = _messageQueue.receive();
      
      if (phase == TrafficLightPhase::green){
        return;
      }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // The private method "cycleThroughPhases“ starts in a thread when the public method "simulate“ is called.
    // Uses the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // Implementation of the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration is a random value between 4 and 6 seconds. 
    // The while-loop uses std::this_thread::sleep_for to wait 1ms between two cycles.

  
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4000, 6000); // cycle duration is random value between 4 and 6 seconds
    long cycleDuration = distr(eng); // duration of a single simulation cycle in ms
  
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true){
      // sleep at every iteration to reduce CPU usage
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

      // compute time difference to stop watch
      long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
      
      if (timeSinceLastUpdate >= cycleDuration) {
      
        if (getCurrentPhase() == TrafficLightPhase::green)
          _currentPhase = TrafficLightPhase::red;
        else
          _currentPhase = TrafficLightPhase::green;
        
        _messageQueue.send(std::move(_currentPhase));
        
        cycleDuration = distr(eng); // pick a new random number
        
        lastUpdate = std::chrono::system_clock::now();
      }
    }
}