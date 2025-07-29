/*
 * filename     : lesOS.ino
 * Copyright    : Copyright (C) 2025 Ansen
 * version      : 1.0.0
 * license      : MIT
 * Creation Date: 2025-07-15
 * Last Updated : 2025-07-15
 * Author       : Ansen
 * Email        : alj79@sohu.com
 *
 * Description: 
 *  lesOS ( lightweight easy simple OS ) is for mcu with less memory.
 *  
 * Notes:
 *  Add new ID in loop() to add a new task for scheduling.
 */

#include <util/atomic.h>
#include <avr/sleep.h>
#include "timer.h"
#include "test.h"
#include "task.h"
#include "bleed.h"

Timer timer;
Test test;
Task task;
Bleed bleed;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  board_init();
  test.on(true);
  bleed.on(true);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

void loop() {
  // put your main code here, to run repeatedly:
  int8_t pri = task.getNextPriority();
  if(pri != -1){
    task.runOnce(TASK_ID_USER); // if there is any task except user's, run user's task once
  }

  switch(pri){
    case TASK_ID_BLEED:
      // Serial.println("[main:] loop(TASK_ID_BLEED)");
      bleed.loop();
      break;

    case TASK_ID_TEST:
      // Serial.println("[main:] loop(TASK_ID_TEST)");
      test.loop();
      break;

    case TASK_ID_USER:
      // Serial.println("[main:] loop(TASK_ID_TEST)");
      // if user's task finish, stop the user's task
      task.stopOnce(TASK_ID_USER);
      break;

    default:
      // Serial.println("[main:] loop(sleep)");
      task.restoreAll();
      if(-1 != task.getNextPriority())  // -1: no task; the Priority is from 0 ~ 7
        sleep_cpu();
      
      break;
  }

}
 
void board_init(void)
{
  noInterrupts(); //禁止所有中断

  task.setup();
  timer.setup();

  test.setup();
  bleed.setup();

  interrupts();               //允许所有中断
  // test.on(true);
}