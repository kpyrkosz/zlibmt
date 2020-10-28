#include <director.hpp>
#include <iostream>

void
director::thread_proc()
{
  for (auto& entry : std::filesystem::directory_iterator(zlibdir_))
  {
    std::unique_lock lock(shared_state_.queue_mutex);
    shared_state_.item_processed.wait(
      lock, [this] { return shared_state_.filepaths_queue.size() < queue_max_size; });

    if (!entry.is_regular_file())
    {
      std::cerr << "Skipping " << entry.path() << ", not a regular file\n";
      continue;
    }
    if (!entry.path().has_extension() || entry.path().extension() != ".zlib")
    {
      std::cerr << "Skipping " << entry.path() << ", not a .zlib extension\n";
      continue;
    }
    shared_state_.filepaths_queue.push(entry.path());
    std::cerr << "Director added a path, queue size: " << shared_state_.filepaths_queue.size()
              << "\n";
    shared_state_.item_added.notify_one();
  }
  // notify workers about the end of job
  std::unique_lock lock(shared_state_.queue_mutex);
  std::cout << "Director exiting, no more files to process\n";
  shared_state_.directory_processed = true;
  shared_state_.item_added.notify_all();
}

director::director(work& shared_state, const std::string& zlibdir)
  : shared_state_(shared_state)
  , zlibdir_(zlibdir)
  , work_thread_(&director::thread_proc, this)
{
}

director::~director()
{
  work_thread_.join();
}
