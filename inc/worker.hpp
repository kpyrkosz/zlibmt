#pragma once

#include <thread>
#include <work.hpp>

class worker
{
  work& shared_state_;
  std::filesystem::path outdir_;
  std::thread work_thread_;

  void thread_proc();

public:
  worker(work& shared_state, const std::string& outdir);
  worker(const worker&) = delete;
  ~worker();
};
