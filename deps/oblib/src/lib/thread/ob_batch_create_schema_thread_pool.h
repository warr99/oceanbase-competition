#ifndef SRC_LIBRARY_SRC_LIB_THREAD_OB_CREATE_SCHEMA_THREAD_POOL_H_
#define SRC_LIBRARY_SRC_LIB_THREAD_OB_CREATE_SCHEMA_THREAD_POOL_H_

#include "lib/queue/ob_fixed_queue.h"
#include "lib/thread/thread_mgr_interface.h"

namespace oceanbase {

class BatchCreateSchemaTask {
public:
  virtual ~BatchCreateSchemaTask() {}
  virtual int process() = 0;
};
class ObBatchCreateSchemaThreadPool : public lib::TGRunnable {
public:
  static const int64_t MAX_TASK_NUM = 1024;
  ObBatchCreateSchemaThreadPool()
      : is_inited_(false), is_stoped_(true), task_queue_() {}
  ~ObBatchCreateSchemaThreadPool() {
    if (!is_stoped_) {
      COMMON_LOG_RET(
          ERROR, OB_ERR_UNEXPECTED,
          "ObBatchCreateSchemaThreadPool is destruction before stop");
      is_stoped_ = true;
    }
    destory();
  }

  int init();
  int start();
  int stop();
  void wait();
  void destory();
  int add_task(BatchCreateSchemaTask *task);
  int pop_task(BatchCreateSchemaTask *&task);

  void run1() override;
  TO_STRING_KV(K_(is_inited), K_(is_stoped), K_(tg_id));

private:
  bool is_inited_;

  bool is_stoped_;
  int tg_id_;
  ObFixedQueue<BatchCreateSchemaTask> task_queue_;
};
} // namespace oceanbase

#endif /* SRC_LIBRARY_SRC_LIB_THREAD_OB_CREATE_SCHEMA_THREAD_POOL_H_ */
