/*
 * filename     : timer.cpp
 * Copyright    : Copyright (C) 2025 Ansen
 * version      : 1.0.0
 * license      : MIT
 * Creation Date: 2025-07-15
 * Last Updated : 2025-07-15
 * Author       : Ansen
 * Email        : alj79@sohu.com
 *
 * Description: 
 *  It provides some functions to delay a few milliseconds.
 *  
 * Notes:
 *  Using timer1;
 *  Array instead of dynamic memory;
 *  Each task with only one part of the array;
 *  Tasks can delay at the same time;
 */

#include <Arduino.h> 
#include "timer.h"
#include <util/atomic.h>

#define TIMER_PRESCALER (1 << CS11)  
#define TIMER_COUNTER 63035
#define TIMER_COUNT_MAX 0xFFFF

extern Task task;

static volatile bool running;
static volatile unsigned int tcount;
static volatile enum task_id_number cur_id;

static volatile struct {
  unsigned int count;
  bool* flag;
  bool blocked;
} timer[TASK_ID_MAX];

static void on(bool b)
{
  if(b){
    TCCR1B |= TIMER_PRESCALER; 
    running = true;
  }
  else{
    TCCR1B &= ~(TIMER_PRESCALER);
    running = false;
  }
}

static enum task_id_number adj_select(unsigned int escape)
{
  unsigned int min = TIMER_COUNT_MAX;
  enum task_id_number current = TASK_ID_MAX;

  for(int8_t i = 0; i < TASK_ID_MAX; i++){
    unsigned int count = timer[i].count;
    
    if(count == TIMER_COUNT_MAX)
      continue; 
      
    if(count < escape) {
      count = 0; 
    } else {
      count -= escape;
    }
    
    timer[i].count = count;
    
    if(min > count){
      min = count;
      current = i;
    }
  }

  return current;
}

void Timer::setup()
{
  running = false;
  tcount = 0;
  cur_id = TASK_ID_MAX;

  for(int8_t i = 0; i < TASK_ID_MAX; i++){
    timer[i].count = TIMER_COUNT_MAX;
    timer[i].flag = NULL;
    timer[i].blocked = false;
  }

  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = TIMER_COUNTER;
  TIMSK1 |= (1 << TOIE1);
}

ISR(TIMER1_OVF_vect) {
  TCNT1 = TIMER_COUNTER;
  
  if(tcount > 0) {
    tcount--;
  } else {
    Serial.println("[timer:] Error: tcount underflow");
    return;
  }

  if (!tcount) {
    enum task_id_number id;
    unsigned int count;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      id = cur_id;
      count = timer[id].count;
      
      if(id < TASK_ID_MAX && timer[id].count != TIMER_COUNT_MAX) {
        timer[id].count = TIMER_COUNT_MAX;
        if(timer[id].flag) {
          *(timer[id].flag) = true;
        }
        
        if(timer[id].blocked) {
          task.clearBlock(id);
        }
      }
      
      id = adj_select(count);
      cur_id = id;
      
      if (id == TASK_ID_MAX) {
        on(false);
      } else {
        tcount = timer[id].count;
      }
    }
  } 
}

void Timer::create(enum task_id_number id, int msecond, bool* flag, bool blocked)
{
  if(id >= TASK_ID_MAX || msecond <= 0) {
    Serial.println("[timer:] Error: invalid parameters");
    return;
  }

  unsigned int count = msecond;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    unsigned int tc =  timer[cur_id].count;
    unsigned int escape = tc - tcount;

    if(cur_id != TASK_ID_MAX && tc != TIMER_COUNT_MAX) {
      count += escape;
    }
    
    timer[id].count = count;
    timer[id].flag = flag;
    timer[id].blocked = blocked;

    enum task_id_number new_id = adj_select(escape);

    cur_id = new_id;
    tcount = timer[new_id].count;
    
    if(blocked) {
      task.setBlock(id);
    }
  }
  
  if(!running) {
    on(true);
  }
}

void Timer::cancel(enum task_id_number id)
{
  if(id >= TASK_ID_MAX) {
    return;
  }

  bool was_active = (id == cur_id);
  timer[id].count = TIMER_COUNT_MAX;
  
  if(timer[id].blocked) {
    task.clearBlock(id);
  }

  if(was_active) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      enum task_id_number new_id = adj_select(tcount);
      cur_id = new_id;
      tcount = timer[new_id].count;
    }
    
    if(cur_id == TASK_ID_MAX) {
      on(false);
    }
  }
}