#include "task.h"
#include <DNSServer.h>
#include <ESPUI.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

const char *ssid = "Trailer_tester";
const char *password = "onthemove";

const char *hostname = "espui";

static const int left_turn_signal_pin = 22;
static const int right_turn_signal_pin = 23;
static const int running_lights_pin = 21;
 
int leftTurnSignalId;
int rightTurnSignalId;
int runningLightsId;

static bool leftTurnSignalState = false;
static bool rightTurnSignalState = false;

static bool leftTurnSignalTestState = false;
static bool rightTurnSignalTestState = false;
static bool runningLightsTestState = false;

// variables for tasks
static const unsigned char num_tasks = 2;
unsigned long current_time = 0;
unsigned long previous_time = 0;
 
// left turn signal task specific
static Task left_turn_signal_blink_task;
unsigned long left_turn_signal_blink_task_period = 500;
enum left_turn_signal_led_blink_states {
    LEFT_TURN_SIGNAL_OFF = 0,
    LEFT_TURN_SIGNAL_ON = 1
};

// right turn signal task specific
static Task right_turn_signal_blink_task;
unsigned long right_turn_signal_blink_task_period = 500;
enum right_turn_signal_led_blink_states {
    RIGHT_TURN_SIGNAL_OFF = 0,
    RIGHT_TURN_SIGNAL_ON = 1
};

void left_turn_signal_test(Control *sender, int value) {
    switch (value) {
        case S_ACTIVE:
            Serial.print("Left turn signal test active:");
            leftTurnSignalTestState = true;

            if (right_turn_signal_blink_task.getState() == RIGHT_TURN_SIGNAL_ON) {
                left_turn_signal_blink_task.setState(LEFT_TURN_SIGNAL_ON);
            } else {
                left_turn_signal_blink_task.setState(LEFT_TURN_SIGNAL_OFF);
            }

            left_turn_signal_blink_task.setElapsedTime(right_turn_signal_blink_task.getElapsedTime());
            break;

        case S_INACTIVE:
            Serial.print("Left turn signal test inactive:");
            leftTurnSignalTestState = false;
            left_turn_signal_blink_task.setState(LEFT_TURN_SIGNAL_OFF);
            digitalWrite(left_turn_signal_pin, HIGH);
            break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void right_turn_signal_test(Control *sender, int value) {
    switch (value) {
        case S_ACTIVE:
            Serial.print("Right turn signal test active:");
            rightTurnSignalTestState = true;

            if (left_turn_signal_blink_task.getState() == LEFT_TURN_SIGNAL_ON) {
                right_turn_signal_blink_task.setState(RIGHT_TURN_SIGNAL_ON);
            } else {
                right_turn_signal_blink_task.setState(RIGHT_TURN_SIGNAL_OFF);
            }

            right_turn_signal_blink_task.setElapsedTime(left_turn_signal_blink_task.getElapsedTime());
            break;

        case S_INACTIVE:
            Serial.print("Right turn signal test inactive:");
            rightTurnSignalTestState = false;
            right_turn_signal_blink_task.setState(LEFT_TURN_SIGNAL_OFF);
            digitalWrite(right_turn_signal_pin, HIGH);
            break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void running_lights_test(Control *sender, int value) {
    switch (value) {
        case S_ACTIVE:
            Serial.print("Running lights test active:");
            runningLightsTestState = true;
            digitalWrite(running_lights_pin, LOW);
            break;

        case S_INACTIVE:
            Serial.print("Running lights test inactive:");
            runningLightsTestState = false;
            digitalWrite(running_lights_pin, HIGH);
            break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

// left turn signal FSM
int left_turn_signal_blink_fsm(int current_state) {
    if (leftTurnSignalTestState) {
        // transitions
        switch (current_state) {
            case LEFT_TURN_SIGNAL_OFF:
                current_state = LEFT_TURN_SIGNAL_ON;
                break;
            case LEFT_TURN_SIGNAL_ON:
                current_state = LEFT_TURN_SIGNAL_OFF;
                break;
            default:
                // do nothing
                break;
        };
  
        // actions
        switch (current_state) {
            case LEFT_TURN_SIGNAL_OFF:
                digitalWrite(left_turn_signal_pin, HIGH);
                break;
            case LEFT_TURN_SIGNAL_ON:
                digitalWrite(left_turn_signal_pin, LOW);
                break;
            default:
                // do nothing
                break;
        };
    };
  
    return current_state;
}

// right turn signal FSM
int right_turn_signal_blink_fsm(int current_state) {
    if (rightTurnSignalTestState) {
        // transitions
        switch (current_state) {
            case RIGHT_TURN_SIGNAL_OFF:
                current_state = RIGHT_TURN_SIGNAL_ON;
                break;
            case RIGHT_TURN_SIGNAL_ON:
                current_state = RIGHT_TURN_SIGNAL_OFF;
                break;
            default:
                // do nothing
                break;
        };
  
        // actions
        switch (current_state) {
            case RIGHT_TURN_SIGNAL_OFF:
                digitalWrite(right_turn_signal_pin, HIGH);
                break;
            case RIGHT_TURN_SIGNAL_ON:
                digitalWrite(right_turn_signal_pin, LOW);
                break;
            default:
                // do nothing
                break;
        };
    };
  
    return current_state;
}

Task* tasks[] = {&left_turn_signal_blink_task, &right_turn_signal_blink_task};

void setup(void) {
    ESPUI.setVerbosity(Verbosity::VerboseJSON);
    Serial.begin(115200);

#if defined(ESP32)
    WiFi.setHostname(hostname);
#else
    WiFi.hostname(hostname);
#endif

    Serial.print("\n\nCreating hotspot");

    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    dnsServer.start(DNS_PORT, "*", apIP);

    Serial.println("\n\nWiFi parameters:");
    Serial.print("Mode: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? "Station" : "Client");
    Serial.print("IP address: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());

    // task 1
    left_turn_signal_blink_task.setState(LEFT_TURN_SIGNAL_OFF);
    left_turn_signal_blink_task.setPeriod(left_turn_signal_blink_task_period);
    left_turn_signal_blink_task.setElapsedTime(left_turn_signal_blink_task_period);
    left_turn_signal_blink_task.setTickFunction(&left_turn_signal_blink_fsm);

    // task 2
    right_turn_signal_blink_task.setState(RIGHT_TURN_SIGNAL_OFF);
    right_turn_signal_blink_task.setPeriod(right_turn_signal_blink_task_period);
    right_turn_signal_blink_task.setElapsedTime(right_turn_signal_blink_task_period);
    right_turn_signal_blink_task.setTickFunction(&right_turn_signal_blink_fsm);

    // set I/O direction for LEDs
    pinMode(left_turn_signal_pin, OUTPUT);
    pinMode(right_turn_signal_pin, OUTPUT);
    pinMode(running_lights_pin, OUTPUT);

    // default setting for LEDs
    digitalWrite(left_turn_signal_pin, HIGH);
    digitalWrite(right_turn_signal_pin, HIGH);
    digitalWrite(running_lights_pin, HIGH);

    leftTurnSignalId = ESPUI.switcher("Left turn signal test", &left_turn_signal_test, ControlColor::Turquoise, leftTurnSignalState);
    rightTurnSignalId = ESPUI.switcher("Right turn signal test", &right_turn_signal_test, ControlColor::Emerald, rightTurnSignalState);
    runningLightsId = ESPUI.switcher("Running lights test", &running_lights_test, ControlColor::Peterriver, runningLightsTestState);

    ESPUI.begin("Trailer signal tester");
}

void loop(void) {
    // Check for request to web server
    dnsServer.processNextRequest();

    // Run the task scheduler
    current_time = millis();

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i]->getElapsedTime() >= tasks[i]->getPeriod()) {
            // tick task
            int task_current_state = tasks[i]->getState();
            int task_new_state = tasks[i]->runTickFunction(task_current_state);

            tasks[i]->setState(task_new_state);
            tasks[i]->setElapsedTime(0);
        }

        tasks[i]->increaseElapsedTime(current_time - previous_time);
    }

    previous_time = current_time;
}
