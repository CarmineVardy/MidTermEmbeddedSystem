/*
 * activity_tracking.c
 *
 *  Created on: Dec 9, 2024
 *      Author: anton
 */

#include "activity_tracking.h"
#include "led.h"
#include "time.h"

//#####################    INITIALIZATION FUNCTIONS    ################################
void StepBuffer_init(StepBuffer *buffer) {
    buffer->index = 0;
    buffer->total_step_time = 0;
    for (int i = 0; i < STEP_BUFFER_SIZE; i++) {
        buffer->steps[i] = 0;
    }
}

void UserActivity_init(UserActivity *activity) {

    activity->steps = 0;
    activity->last_step_time = 0;
    activity->t1 = 0;
    activity->right_sensor = 100000;
    activity->left_sensor =  100000;
    activity->step_detection = 0;
    activity->state = RESTING;
    activity->last_state_time = 0;
}

//#####################    READING  FUNCTIONS    ################################
const char* userState_to_string(UserState state) {
    switch (state) {
        case RESTING: return "RESTING";
        case WALKING: return "WALKING";  a
        case RUNNING: return "RUNNING";
        default: return "UNKNOWN";
    }
}

void read_forceSensor(uint16_t *d_out, float *sensor) {
    float voltage;
    voltage = ((float)(*d_out)) * VREF / LEVELS;
    if (voltage < 150)
        voltage = 0;

    *sensor = (VREF * RM - voltage * RM) / voltage;
}



//#####################    PEDOMETER    ################################

void step_counter(UserActivity *activity , StepBuffer *buffer) {
    uint32_t current_time = HAL_GetTick();
    activity->step_detection = 0;

    if (activity->right_sensor <= STEP_TRESHOLD || activity->left_sensor < STEP_TRESHOLD) {
        if (activity->step_detection == 0) {
            activity->step_detection = 1;
            activity->steps++;

            uint32_t step_time = current_time - activity->last_step_time;


            buffer->steps[buffer->index] = step_time;
            buffer->index = (buffer->index + 1) % STEP_BUFFER_SIZE;


            buffer->total_step_time = 0;
            for (int i = 0; i < STEP_BUFFER_SIZE; i++) {
                buffer->total_step_time += buffer->steps[i];
            }

            activity->last_step_time = current_time;
        }
    } else {
        activity->step_detection = 0;
    }
}



//#####################    USER'S STATE OF ACTIVITY DETECTING    ################################

void detect_activity(UserActivity *activity, StepBuffer *buffer, ledConfig *red_led, ledConfig *green_led, ledConfig *yellow_led) {
    uint32_t current_time = HAL_GetTick();





    //USER'S STATE: RESTING
    if ((activity->right_sensor <= STEP_TRESHOLD) && (activity->left_sensor <= STEP_TRESHOLD)) {
        if (activity->t1 == 0) {
            activity->t1 = current_time;
        }
        if ((current_time - activity->t1) >= RESTING_MIN_TIME) {
            activity->state = RESTING;
            turnOFF_led(red_led);
            turnON_led(green_led);
            turnOFF_led(yellow_led);
        }
    } else {
        // Led and time reset if any resting state is detected
        activity->t1 = 0;
        turnOFF_led(red_led);
        turnOFF_led(green_led);
        turnOFF_led(yellow_led);
    }





    //USER'S STATE: MOVING
    /*This function ,in order to understand if the user is walking or running,calculates the average of steps_buffer times.
    This average indicates the average time beetween two steps. If it is under the "Running Treshold "it means that user is running
    otherwise it means that user is walking
    */
    if (buffer->total_step_time > 0) {
        uint32_t avg = buffer->total_step_time / STEP_BUFFER_SIZE;

        if (avg <= RUNNING_STEP_THRESHOLD) {
            // RUNNING
            activity->state = RUNNING;
            turnON_led(red_led);
            turnOFF_led(green_led);
            turnOFF_led(yellow_led);
        } else {
            // WALKING
            activity->state = WALKING;
            turnON_led(yellow_led);
            turnOFF_led(green_led);
            turnOFF_led(red_led);
        }
    }
}





