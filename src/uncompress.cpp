#include <cassert>
#include <string>
#include <uncompress.hpp>
#include <zlib.h>

namespace extract
{

// holds a reference to initialized zstream and releases it as it goes out of scope
struct z_stream_scope_guard
{
  z_stream& stream_to_free_;
  z_stream_scope_guard(z_stream& stream_to_free)
    : stream_to_free_(stream_to_free)
  {
  }
  ~z_stream_scope_guard() { (void)inflateEnd(&stream_to_free_); }
};

void
uncompress_stream(std::istream& compressed_stream, std::ostream& inflate_to)
{
  z_stream strm{};
  if (auto init_retval = inflateInit(&strm); init_retval != Z_OK)
    throw std::runtime_error("Zlib init failed with error code " + std::to_string(init_retval));

  z_stream_scope_guard zstream_releaser(strm);

  for (int ret = Z_OK; ret != Z_STREAM_END;)
  {
    static constexpr unsigned chunk_size = 2 << 16;
    // suck a chunk
    unsigned char in[chunk_size];
    compressed_stream.read(reinterpret_cast<char*>(in), chunk_size);
    std::streamsize last_read_charcount = compressed_stream.gcount();

    // sanity check for signed cast and truncation
    assert(static_cast<unsigned long long>(last_read_charcount) <= chunk_size);

    strm.avail_in = static_cast<unsigned>(last_read_charcount);
    if (strm.avail_in == 0 && !compressed_stream)
      throw std::runtime_error("Read from compressed_stream failed");

    strm.next_in = in;
    while (strm.avail_in != 0)
    {
      // inflate the chunk
      unsigned char out[chunk_size];
      strm.avail_out = chunk_size;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
      if (ret != Z_OK && ret != Z_STREAM_END)
        throw std::runtime_error("Zlib decompression returned error code " + std::to_string(ret));

      // write the uncompressed part to the output stream
      inflate_to.write(reinterpret_cast<char*>(out), chunk_size - strm.avail_out);
      if (!inflate_to)
        throw std::runtime_error("Write to output stream failed");
    }
  }
}

} // namespace extract
