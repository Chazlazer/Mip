#include <iostream>
#include <iomanip>
#include <Python.h>
#include <string.h>
#include <thread>
#include <signal.h>
#include <chrono>
#include <csignal>
#include <cstdint>

#include <Arduino_wrapper.h>
#include "robotcontrol.h"

#include "pid.h"
#include "lowpass_filter.h"
#include "Encoder.h"
#include "foc_utils.h"
#include "joystick_commands.h"

#define I2C_BUS 2
#define GPIO_INT_PIN_CHIP 3
#define GPIO_INT_PIN_PIN  21

#define RIGHT_MOTOR 3
#define LEFT_MOTOR 1

rc_mpu_data_t data;

Encoder encoder1 = Encoder(2,533.655);
Encoder encoder2 = Encoder(3,533.655);

PIDController pid_stb = PIDController(3.0,5.0, 0.001, 100000, 0.7);
PIDController pid_vel = PIDController(0.01, 0.0, 0.0, 10000, _PI/10);

LowPassFilter lpf_pitch_cmd{0.07};
LowPassFilter lpf_throttle{0.5};
LowPassFilter lpf_steering{0.1};

Commands cmd;

volatile sig_atomic_t keepRunning = 1;

float compute_velocity_cmd();

void signalHandler(int) {
    rc_set_state(EXITING);
}

int main(){
    // Python Code Setup

    signal(SIGINT, signalHandler);

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('.')");

    PyObject* pName = PyUnicode_DecodeFSDefault("bt_server");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (!pModule) {
        PyErr_Print();
        std::cerr << "Failed to load bt_server.py" << std::endl;
        Py_Finalize();
        return 1;
    }

    PyObject* pInitFunc = PyObject_GetAttrString(pModule, "init_server");
    if (!pInitFunc || !PyCallable_Check(pInitFunc)) {
        PyErr_Print();
        std::cerr << "Cannot find init_server()" << std::endl;
        Py_XDECREF(pInitFunc);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }
    std::cout << "Starting Bluetooth server..." << std::endl;
    PyObject* pInitResult = PyObject_CallObject(pInitFunc, nullptr);
    if (!pInitResult) {
        PyErr_Print();
        std::cerr << "init_server() failed" << std::endl;
        Py_DECREF(pInitFunc);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }
    Py_XDECREF(pInitResult);
    Py_DECREF(pInitFunc);

    PyObject* pPollFunc = PyObject_GetAttrString(pModule, "poll_data");
    PyObject* pGetFunc = PyObject_GetAttrString(pModule, "get_last_command_dict");

    if (!pPollFunc || !PyCallable_Check(pPollFunc) ||
        !pGetFunc || !PyCallable_Check(pGetFunc)) {
        PyErr_Print();
        std::cerr << "Cannot find poll_data() or get_last_command_dict()" << std::endl;
        Py_XDECREF(pPollFunc);
        Py_XDECREF(pGetFunc);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    // Map Python keys to struct members
    const char* keys[] = {
        "BUTTON_A", "BUTTON_B", "BUTTON_X", "BUTTON_Y",
        "BUTTON_L", "BUTTON_R", "BUTTON_WL", "BUTTON_WR",
        "JOYSTICK_L_X", "JOYSTICK_L_Y",
        "JOYSTICK_R_X", "JOYSTICK_R_Y",
        "TRIGGER_L", "TRIGGER_R"
    };

    // Setup Encoders
    encoder1.init();
    encoder2.init();

    // Setup MPU DMP
    rc_mpu_config_t conf = rc_mpu_default_config();
    conf.i2c_bus = I2C_BUS;
    conf.gpio_interrupt_pin_chip = GPIO_INT_PIN_CHIP;
    conf.gpio_interrupt_pin = GPIO_INT_PIN_PIN;
    conf.enable_magnetometer = true;
    conf.dmp_sample_rate = 200;
    rc_mpu_initialize_dmp(&data, conf);

    // Setup Motors
    rc_motor_init();

    rc_set_state(RUNNING);
    while (rc_get_state()!=EXITING)
    {
        // Update Commands from joystick
        // Call poll_data()
        PyObject* pPollResult = PyObject_CallObject(pPollFunc, nullptr);
        Py_XDECREF(pPollResult);

        // Get last_command dict
        PyObject* pDict = PyObject_CallObject(pGetFunc, nullptr);
        if (pDict && PyDict_Check(pDict)) {
            // Assign values to struct
            cmd.BUTTON_A   = PyLong_AsLong(PyDict_GetItemString(pDict, keys[0]));
            cmd.BUTTON_B   = PyLong_AsLong(PyDict_GetItemString(pDict, keys[1]));
            cmd.BUTTON_X   = PyLong_AsLong(PyDict_GetItemString(pDict, keys[2]));
            cmd.BUTTON_Y   = PyLong_AsLong(PyDict_GetItemString(pDict, keys[3]));
            cmd.BUTTON_L   = PyLong_AsLong(PyDict_GetItemString(pDict, keys[4]));
            cmd.BUTTON_R   = PyLong_AsLong(PyDict_GetItemString(pDict, keys[5]));
            cmd.BUTTON_WL  = PyLong_AsLong(PyDict_GetItemString(pDict, keys[6]));
            cmd.BUTTON_WR  = PyLong_AsLong(PyDict_GetItemString(pDict, keys[7]));
            cmd.JOYSTICK_L_X = (uint8_t)PyLong_AsLong(PyDict_GetItemString(pDict, keys[8]));
            cmd.JOYSTICK_L_Y = (uint8_t)PyLong_AsLong(PyDict_GetItemString(pDict, keys[9]));
            cmd.JOYSTICK_R_X = (uint8_t)PyLong_AsLong(PyDict_GetItemString(pDict, keys[10]));
            cmd.JOYSTICK_R_Y = (uint8_t)PyLong_AsLong(PyDict_GetItemString(pDict, keys[11]));
            cmd.TRIGGER_L    = (uint8_t)PyLong_AsLong(PyDict_GetItemString(pDict, keys[12]));
            cmd.TRIGGER_R    = (uint8_t)PyLong_AsLong(PyDict_GetItemString(pDict, keys[13]));
        } else {
            PyErr_Print();
        }
        Py_XDECREF(pDict);

        // Manipulate Commands
        float velocity_cmd = compute_velocity_cmd();

        encoder1.update();
        encoder2.update();

        float pitch = data.fused_TaitBryan[TB_PITCH_X] - 60*DEG_TO_RAD;
        float vel_error = (encoder1.getVelocity() + encoder2.getVelocity())/2  - lpf_throttle(velocity_cmd);

        float vel_output = pid_vel(vel_error);

        float target_pitch = lpf_pitch_cmd(vel_output);
        
        float pitch_error = target_pitch - pitch;
        float voltage_control = pid_stb(pitch_error);

        std::cout << std::fixed << std::setprecision(2)<< "VelErr: " << vel_error <<" Target Vel:" << vel_output << " Target Pitch: " << target_pitch * RAD_TO_DEG 
        << " Pitch: " << pitch * RAD_TO_DEG << " EncVel: " << encoder1.getVelocity() << " | " << encoder2.getVelocity()
        << " Stb Cmd: " << voltage_control << " Cmd: " << velocity_cmd<< std::endl;
        
        rc_motor_set(RIGHT_MOTOR, voltage_control);
        rc_motor_set(LEFT_MOTOR , -voltage_control);


        // std::cout << "Encoder1: " << encoder1.getAngle() << " Rad" << std::endl;

        // delayMicroseconds(1000);
        delay(10);
    }
    std::cout << "\nStopping..." << std::endl;
    Py_XDECREF(pPollFunc);
    Py_XDECREF(pGetFunc);
    Py_DECREF(pModule);
    Py_Finalize();

    return 0;

}
float compute_velocity_cmd(){
    float trigR = mapfloat(cmd.TRIGGER_R,0.0,180.0,0.0,20);
    float trigL = mapfloat(cmd.TRIGGER_L,0.0,180.0,0.0,20);
    float velocity_cmd = fmin(trigL,20.0) - fmin(trigR,20.0);
    return velocity_cmd;
}
    // pinA.open(GPIO1_17,OUTPUT);

    // while(count < 10){
    //     auto curr_time = millis();
    //     if((curr_time - start_time) > blinkDelay){
    //         start_time = curr_time;
    //         if(ledState == LOW){
    //             ledState = HIGH;
    //             count++;
    //         }
    //         else{
    //             ledState = LOW;
    //             count++;
    //         }
    //         rc_led_set(RC_LED_GREEN, ledState);
    //         pinA.write(GPIO1_17, ledState);
    //         std::cout << "Loop Count: " << count << std::endl;
    //     }
    // }