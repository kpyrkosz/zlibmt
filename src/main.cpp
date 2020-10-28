#include <director.hpp>
#include <iostream>
#include <memory>
#include <worker.hpp>

int
main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <dir with *.zlib files> <out dir>\n";
    return 1;
  }
  try
  {
    work shared_state;
    director dir(shared_state, argv[1]);
    std::vector<std::unique_ptr<worker>> workers;
    for (int i = 0; i < 8; ++i)
      workers.emplace_back(std::make_unique<worker>(shared_state, argv[2]));
    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
