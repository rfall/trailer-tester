#include "task.h"
  
/**
 * Constructor
 * Set variables to default values here.
 */
Task::Task() {
    // do nothing
}
  
/**
 * function: setState
 * This method set the state of the task to the state
 * passed in.
 */
void Task::setState(const signed char & new_state) {
    state = new_state;
}
  
/**
 * function: getState
 * This method returns the current state of the task.
 */
signed char Task::getState() {
    return state;
}
  
/**
 * function: setPeriod
 * This method sets the period of the task. This influences
 * how often the finite state machine ticks.
 */
void Task::setPeriod(const unsigned long & new_period) {
    period = new_period;
}
  
/**
 * function: getPeriod
 * This method returns the period of the task.
 */
unsigned long Task::getPeriod() {
    return period;
}
  
/**
 * function: setElapsedTime
 * This method sets the time elapsed for the task. Use for initialization
 * and debugging.
 */
void Task::setElapsedTime(const unsigned long & time_elapsed) {
    elapsed_time = time_elapsed;
}
  
/**
 * function: getElapsedTime
 * This method returns the time elapsed since the last time the finite
 * state machine ticks.
 */
unsigned long Task::getElapsedTime() {
    return elapsed_time;
}
  
/**
 * function: increaseElapsedTime
 * This method adds a small amount of time to the time elapsed. The small
 * amount of time is the interval between the last check time and current.
 */
void Task::increaseElapsedTime(const unsigned long & delta_time) {
    elapsed_time += delta_time;
}
  
/**
 * function: setTickFunction
 * This method sets the function to invoke when it is time for task action to tick.
 * Ideally, the function is a FSM, but it can be any function that fits the parameter
 * and return type.
 */
void Task::setTickFunction(int (*tick_function)(int)) {
    TickFct = tick_function;
}
  
/**
 * function: runTickFunction
 * This method invokes the function (FSM) of this task.
 */
signed char Task::runTickFunction(const signed char & current_state) {
    return TickFct(current_state);
}
  
/**
 * Destructor
 * This gets invoke when the program closes to do clean up. Free or delete
 * any dynamically allocated variables here.
 */
Task::~Task()
{
    // do nothing
}
