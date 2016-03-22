//
// Copyright 2016 imai hiroyuki.
// All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
#ifndef PETTYUTIL_TASK_H_
#define PETTYUTIL_TASK_H_

#include <memory>

#include "petty_helper.h"

namespace pettyutil {

/// Runnable interface.
///
class Runnable {
 public:
  /// Destruct runnable interface.
  ///
  virtual ~Runnable() {}

  /// Run interface.
  ///
  virtual void Run() = 0;
};

/// Task class.
///
class Task : public Runnable {
 public:
  /// Construct task.
  ///
  /// @param command command to run.
  ///
  inline explicit Task(std::shared_ptr<Runnable> command)
      : command_(command) {}

  /// Destruct task.
  ///
  virtual ~Task() {}

  /// Set continue task.
  ///
  /// @param command command to run.
  /// @return new task.
  ///
  inline std::shared_ptr<Task> ContinueWith(std::shared_ptr<Runnable> command) {
    std::shared_ptr<Task> task(new Task(command));

    task->continue_ = continue_;
    continue_ = task;

    return task;
  }

  /// Swap task.
  ///
  /// @param task swap task.
  ///
  inline void Swap(Task* task) {
    if (task != nullptr) {
      command_.swap(task->command_);
      continue_.swap(task->continue_);
    }
  }

  /// Implement of Runnable::Run method.
  ///
  virtual void Run() {
    if (command_) {
      // run command
      command_->Run();
    }

    if (continue_) {
      // continue task
      continue_->Run();
    }
  }

 private:
  /// Task command.
  std::shared_ptr<Runnable> command_;
  /// Continue task.
  std::shared_ptr<Task> continue_;

  // non-copyable class.
  DISALLOW_COPY_AND_ASSIGN(Task);
};

/// Make task.
///
/// @param command command to run.
/// @return new task.
///
inline std::shared_ptr<Task> MakeTask(Runnable* command) {
  return std::shared_ptr<Task>(new Task(std::shared_ptr<Runnable>(command)));
}

/// Make task.
///
/// @param command command to run.
/// @return new task.
///
inline std::shared_ptr<Task> MakeTask(std::shared_ptr<Runnable> command) {
  return std::shared_ptr<Task>(new Task(command));
}

}  // namespace pettyutil

#endif  // PETTYUTIL_TASK_H_
