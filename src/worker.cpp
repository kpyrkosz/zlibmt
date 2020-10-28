#include <iostream>
#include <uncompress.hpp>
#include <worker.hpp>

void
worker::thread_proc()
{
  while (true)
  {
    std::unique_lock lock(shared_state_.queue_mutex);
    shared_state_.item_added.wait(lock, [this] {
      return shared_state_.directory_processed || !shared_state_.filepaths_queue.empty();
    });
    if (!shared_state_.filepaths_queue.empty())
    {
      // pop the string
      auto file_to_process = std::move(shared_state_.filepaths_queue.front());
      shared_state_.filepaths_queue.pop();
      lock.unlock();
      // now, we can process it locally and let other thread get their strings
      std::ifstream filein(file_to_process, std::ios::binary);
      std::ofstream fileout(outdir_ / (file_to_process.filename().string() + ".out"),
                            std::ios::binary);
      try
      {
        extract::uncompress_stream(filein, fileout);
        lock.lock();
        std::cout << "Uncompressed file \"" << file_to_process << "\"\n";
      }
      catch (const std::runtime_error& e)
      {
        lock.lock();
        std::cerr << "Uncompressing file \"" << file_to_process << "\" threw an exception "
                  << e.what() << '\n';
      }
      // notify the manager
      shared_state_.item_processed.notify_one();
    }
    else if (shared_state_.directory_processed)
    {
      std::cerr << "Thread: " << std::this_thread::get_id()
                << " bailing out, directory processed!\n";
      return;
    }
  }
}

worker::worker(work& shared_state, const std::string& outdir)
  : shared_state_(shared_state)
  , outdir_(outdir)
  , work_thread_(&worker::thread_proc, this)
{
}

worker::~worker()
{
  work_thread_.join();
}
