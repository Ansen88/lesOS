# lesOS
This is a OS for arduino. It's lightweight easy and simple. And it has been tested on the Arduino Pro Min (ATmega328P 5v).

# Introduction
lesOS ( lightweight easy simple OS ) is for mcu with less memory. 

***It's without dynamic memory management.***
We cannot use malloc/free. Wherever dynamic memory allocation is required, arrays are used instead. So we must understand and plan the tasks in advance, and modify the corresponding parameters in the system， eg:timer.

***It's with a simple scheduler based on priority.***
A summary which is one byte records every task's priority, So there are 8 priorities for tasks. If the bit set 1, the corresponding task can work. 

This system is designed for MCUs with less memory, so no corresponding stack is allocated for each task. Therefore, although the system has a priority-based scheduler, it is not a preemptive operating system - scheduling occurs only after a task returns or exits. Thus, it is recommended to use the state machine pattern in task design: decompose the task into multiple small stages; after each small stage is completed, return to the scheduler. This way, certain real-time requirements can be met.

***It's with a timer.***
It provides some functions to delay a few milliseconds.
In this part, each task corresponds to one part of the array - we use the array instead of the dynamic memory. Tasks can be delayed at the same time. And the timer works in the order of expiration. When a task is delay for a while, it can be blocded or not.

***It's with a simple task management***
There is only one state for task, which is "block". A task can be blocked, and can be run once at blocked state, and so on.

***It's with a simple power management***
When there is no any task running, the system can into a idle state. You can modify the flag "SLEEP_MODE_IDLE" to into other sleep mode.

# Test

Currently, tests are conducted by creating "test" and "bleed" tasks in the system. In each task, a message is printed after a certain delay set by a timer.

```shell
15:40:54.436 -> [test:] loop(true)
15:40:55.693 -> [test:] loop(true)
15:40:55.693 -> [Bleed:] loop(true)
15:40:56.954 -> [test:] loop(true)
15:40:58.210 -> [test:] loop(true)
15:40:58.211 -> [Bleed:] loop(true)
15:40:59.470 -> [test:] loop(true)
15:41:00.727 -> [test:] loop(true)
15:41:00.727 -> [Bleed:] loop(true)
15:41:01.953 -> [test:] loop(true)
15:41:03.211 -> [test:] loop(true)
15:41:03.211 -> [Bleed:] loop(true)
15:41:04.464 -> [test:] loop(true)
15:41:05.719 -> [test:] loop(true)
15:41:05.719 -> [Bleed:] loop(true)
15:41:06.978 -> [test:] loop(true)
```

# How to use

***create task***

Refer to the `test` related code in the `lesOS` project.

***Modify some code***

+ Add 'ID' in `enum task_id_number` @ `task.h`
+ Add the ID branch in `loop()` @ `lesOS.ino`
