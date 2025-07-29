/*
 * filename     : task.cpp
 * Copyright    : Copyright (C) 2025 Ansen
 * version      : 1.0.0
 * license      : MIT
 * Creation Date: 2025-07-15
 * Last Updated : 2025-07-15
 * Author       : Ansen
 * Email        : alj79@sohu.com
 *
 * Description: 
 *  It provides a scheduler based on priority and task's state(block).
 *  
 * Notes:
 *  The functions xxxOnce() can let the task to be selected by scheduler. 
 *  And when no task is selected, the scheduler will resume tasks according to the state(block).
 */

#include <Arduino.h>
#include "task.h"
#include <util/atomic.h>

static volatile uint8_t taskPriority;
static const int8_t ffsTable[256] = {
  -1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

static struct {
  bool block : 1;
  bool block_ : 1;
} status[TASK_ID_MAX];

void Task::setup() {
  taskPriority = 0;
}

int8_t Task::getNextPriority() {
  uint8_t ffs;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    ffs = ffsTable[taskPriority];
  }
  return ffs;
}

void Task::setBlock(enum task_id_number id) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    status[id].block = true;
    status[id].block_ = true;
    taskPriority &= ~(1 << id);
  }
}

void Task::clearBlock(enum task_id_number id) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    status[id].block = false;
    status[id].block_ = false;
    taskPriority |= 1 << id;
  }
}

void Task::runOnce(enum task_id_number id) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    taskPriority |= 1 << id;
  }
}

void Task::stopOnce(enum task_id_number id) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    taskPriority &= ~(1 << id);
  }
}

void Task::skipTask(enum task_id_number id) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (status[id].block_) {
    }
  }
}

void Task::restoreAll() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    for (int i = 0; i < TASK_ID_MAX; i++) {
      if (status[i].block) {
        taskPriority &= ~(1 << i);
      } else {
        taskPriority |= 1 << i;
      }
    }
  }
}