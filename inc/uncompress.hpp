#pragma once

#include <fstream>

namespace extract
{
void uncompress_stream(std::istream& compressed_stream, std::ostream& inflate_to);
}
