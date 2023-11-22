#define USING_LOG_PREFIX BOOTSTRAP
#include "ob_batch_create_schema_thread_pool.h"
#include "lib/queue/ob_fixed_queue.h"
#include "share/ob_thread_mgr.h"

namespace oceanbase {

using namespace lib;
using namespace common;

int ObBatchCreateSchemaThreadPool::init() {
  LOG_INFO("start init batch create schema thread pool");
  int ret = OB_SUCCESS;
  if (IS_INIT) {
    ret = OB_INIT_TWICE;
    LOG_WARN("init twice", K(ret));
  } else if (OB_FAIL(task_queue_.init(MAX_TASK_NUM))) {
    COMMON_LOG(WARN, "failed to init task queue", K(ret));
  } else if (OB_FAIL(TG_CREATE_TENANT(TGDefIDs::BATCH_CREATE_SCHEMA_THREAD,
                                      tg_id_))) {
    LOG_WARN("init batch create schema thread pool failed");
  } else {
    is_inited_ = true;
    is_stoped_ = true;
  }
  return ret;
}
int ObBatchCreateSchemaThreadPool::start() {
  LOG_INFO("start batch create schema thread pool");
  int ret = OB_SUCCESS;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("batch create schema thread pool not init", K(ret), KP(this));
  } else if (OB_FAIL(TG_SET_RUNNABLE_AND_START(tg_id_, *this))) {
    LOG_WARN("start batch create schema thread pool failed", K(ret));
  }
  is_stoped_ = false;
  return ret;
}
int ObBatchCreateSchemaThreadPool::stop() {
  while (task_queue_.get_curr_total() != 0) {
    ob_usleep(100 * 1000L);
  }
  is_stoped_ = true;
  return OB_SUCCESS;
}
void ObBatchCreateSchemaThreadPool::wait() {
  // TODO
  LOG_INFO("unrealized");
}
void ObBatchCreateSchemaThreadPool::destory() {
  is_stoped_ = true;
  if (is_inited_) {
    stop();
    wait();
    is_inited_ = false;
    LOG_INFO("batch create schema thread pool is destroyed", K(*this));
  }
}
int ObBatchCreateSchemaThreadPool::add_task(BatchCreateSchemaTask *task) {
  int ret = OB_SUCCESS;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else if (OB_FAIL(task_queue_.push(task))) {
    LOG_WARN("failed to push task", K(ret));
  }
  return ret;
}
int ObBatchCreateSchemaThreadPool::pop_task(BatchCreateSchemaTask *&task) {
  int ret = OB_SUCCESS;
  task = NULL;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("not inited", K(ret));
  } else if (is_stoped_) {
    ret = OB_IN_STOP_STATE;
  } else if (OB_FAIL(task_queue_.pop(task))) {
    if (OB_ENTRY_NOT_EXIST != ret) {
      LOG_WARN("failed to pop task", K(ret));
    }
  }
  return ret;
}
void ObBatchCreateSchemaThreadPool::run1() {

  int tmp_ret = OB_SUCCESS;
  BatchCreateSchemaTask *task;
  while (!is_stoped_) {
    task = NULL;
    if (OB_SUCCESS != (tmp_ret = pop_task(task))) {
      if (OB_ENTRY_NOT_EXIST == tmp_ret) {
        // TODO: set thread idle
      } else if (OB_IN_STOP_STATE == tmp_ret) {
        break;
      } else {
        COMMON_LOG_RET(WARN, tmp_ret, "failed to pop batch create schema task",
                       K(tmp_ret));
      }
    } else if (OB_SUCCESS != (tmp_ret = task->process())) {
      COMMON_LOG_RET(WARN, tmp_ret,
                     "failed to process batch create schema task");
    }
  }
}

} // namespace oceanbase
