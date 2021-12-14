#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <future>

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> _ulock(_mutex);
    _condition.wait(_ulock, [this] { return !_queue.empty(); });

    T message = std::move(_queue.front());
    _queue.pop_front();
    
    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> _ulock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    queue = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

TrafficLight::~TrafficLight()
{

}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if(queue->receive() == TrafficLightPhase::green)
        {
            return;
        }
        //for(auto message = queue->_queue)
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    //double cycleDuration = rand()%3 + 4;
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int> distr(4000, 6000);
    long int cycleDuration = distr(eng);
    auto last_update = std::chrono::system_clock::now();
    std::vector<std::future<void>> futures;
    //std::shared_ptr<MessageQueue<TrafficLightPhase>> queue(new MessageQueue<TrafficLightPhase>);
    //queue = std::make_shared<MessageQueue<TrafficLightPhase>>();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long int timesincelastupdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_update).count();
        if(timesincelastupdate >= cycleDuration)
        {
            if(_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
                //futures.emplace_back(std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, queue, std::move(_currentPhase)));
            }
            else{
                _currentPhase = TrafficLightPhase::red;
                //futures.emplace_back(std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, queue, std::move(_currentPhase)));
            }
            queue->send(std::move(_currentPhase));
            last_update = std::chrono::system_clock::now();
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<int> distr(4000, 6000);
            long int cycleDuration = distr(eng);
        }

        //last_update = std::chrono::system_clock::now();
    }
}
