#pragma once

#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <queue>

struct work
{
  // input queue, director adds files to process in there
  std::queue<std::filesystem::path> filepaths_queue;
  std::mutex queue_mutex;
  // cv awaited by the workers, signaled by the director
  std::condition_variable item_added;
  // cv awaited by the director, signaled by the workers after processing single string
  std::condition_variable item_processed;
  bool directory_processed = false;
};
