#ifndef TASKS_H
#define TASKS_H

void empty_task_199(void *pvParameters);
void communication_task(void *pvParameters);
void receive_task(void *pvParameters);
void set_vin_task(void *pvParameters);
void set_immo_task(void *pvParameters);
void clear_dtc_task(void *pvParameters);

#endif // TASKS_H