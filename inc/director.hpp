#pragma once

#include <thread>
#include <work.hpp>

class director
{
  static const unsigned queue_max_size = 20;

  work& shared_state_;
  std::filesystem::path zlibdir_;
  std::thread work_thread_;

  void thread_proc();

public:
  director(work& shared_state, const std::string& zlibdir);
  director(const director&) = delete;
  ~director();
};
