# lesOS
This is a OS for arduino. It's lightweight easy and simple. And it has been tested on the Arduino Pro Min (ATmega328P 5v).

# Introduction
lesOS ( lightweight easy simple OS ) is for mcu with less memory. 

***It's without dynamic memory management.***
We cannot use malloc/free. Wherever dynamic memory allocation is required, arrays are used instead. So we must understand and plan the tasks in advance, and modify the corresponding parameters in the system， eg:timer.

***It's with a simple scheduler based on priority.***
A summary which is one byte records every task's priority, So there are 8 priorities for tasks. If the bit set 1, the corresponding task can work. 
Task scheduling occurs between one task ends and another task begins, so tasks cannot be preempted unitl the current task actively return (exit).

***It's with a timer.***
It provides some functions to delay a few milliseconds.
In this part, each task corresponds to one part of the array - we use the array instead of the dynamic memory. Tasks can be delayed at the same time. And the timer works in the order of expiration. When a task is delay for a while, it can be blocded or not.

***It's with a simple task management***
There is only one state for task, which is "block". A task can be blocked, and can be run once at blocked state, and so on.

***It's with a simple power management***
When there is no any task running, the system can into a idle state. You can modify the flag "SLEEP_MODE_IDLE" to into other sleep mode.

