#ifndef REPORT_TASK_H
#define REPORT_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern TaskHandle_t xReportTaskHandle;

void vReportTask(void *pvParameters);

#endif /* REPORT_TASK_H */
