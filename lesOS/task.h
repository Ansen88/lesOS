#ifndef _TASK_H_
#define _TASK_H_
enum task_id_number{
  // task's device
  TASK_ID_BLEED,
  TASK_ID_TEST,

  // task's user
  TASK_ID_USER,
  TASK_ID_MAX,
};

class Task
{
  public:
    void setup();
    int8_t getNextPriority();
    void setBlock(enum task_id_number id);
    void clearBlock(enum task_id_number id);
    void runOnce(enum task_id_number id);
    void stopOnce(enum task_id_number id);
    void skipTask(enum task_id_number id);
    void restoreAll();
};
#endif