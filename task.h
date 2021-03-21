#ifndef _TASK_H_
#define _TASK_H_
#endif
  
class Task {
    private:
        unsigned long period;
        unsigned long elapsed_time;
        signed char state;
        int (*TickFct)(int);
    public:
        Task();
        void setState(const signed char &);
        signed char getState();
        void setPeriod(const unsigned long &);
        unsigned long getPeriod();
        void setElapsedTime(const unsigned long &);
        unsigned long getElapsedTime();
        void increaseElapsedTime(const unsigned long &);
        void setTickFunction(int (*tick_function)(int));
        signed char runTickFunction(const signed char &);
        ~Task();
};
