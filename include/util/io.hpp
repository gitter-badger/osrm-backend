#ifndef OSRM_INCLUDE_UTIL_IO_HPP_
#define OSRM_INCLUDE_UTIL_IO_HPP_

#include "util/simple_logger.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <cstddef>
#include <cstdint>

#include <bitset>
#include <vector>

namespace osrm
{
namespace util
{

inline bool serializeFlags(const boost::filesystem::path &path, const std::vector<bool> &flags)
{
    // TODO this should be replaced with a FILE-based write using error checking
    std::uint32_t number_of_bits = flags.size();
    boost::filesystem::ofstream flag_stream(path, std::ios::binary);
    flag_stream.write(reinterpret_cast<const char *>(&number_of_bits), sizeof(number_of_bits));
    // putting bits in ints
    std::uint32_t chunk = 0;
    std::size_t chunk_count = 0;
    for (std::size_t bit_nr = 0; bit_nr < number_of_bits;)
    {
        std::bitset<32> chunk_bitset;
        for (std::size_t chunk_bit = 0; chunk_bit < 32 && bit_nr < number_of_bits;
             ++chunk_bit, ++bit_nr)
            chunk_bitset[chunk_bit] = flags[bit_nr];

        chunk = chunk_bitset.to_ulong();
        ++chunk_count;
        flag_stream.write(reinterpret_cast<const char *>(&chunk), sizeof(chunk));
    }
    util::SimpleLogger().Write() << "Wrote " << number_of_bits << " bits in " << chunk_count
                                 << " chunks (Flags).";
    return flag_stream;
}

inline bool deserializeFlags(const boost::filesystem::path &path, std::vector<bool> &flags)
{
    util::SimpleLogger().Write() << "Reading flags from " << path;
    std::uint32_t number_of_bits;
    boost::filesystem::ifstream flag_stream(path, std::ios::binary);
    flag_stream.read(reinterpret_cast<char *>(&number_of_bits), sizeof(number_of_bits));
    flags.resize(number_of_bits);
    // putting bits in ints
    std::uint32_t chunks = (number_of_bits + 31) / 32;
    std::size_t bit_position = 0;
    std::uint32_t chunk;
    for (std::size_t chunk_id = 0; chunk_id < chunks; ++chunk_id)
    {
        flag_stream.read(reinterpret_cast<char *>(&chunk), sizeof(chunk));
        std::bitset<32> chunk_bits(chunk);
        for (std::size_t bit = 0; bit < 32 && bit_position < number_of_bits; ++bit, ++bit_position)
            flags[bit_position] = chunk_bits[bit];
    }
    util::SimpleLogger().Write() << "Read " << number_of_bits << " bits in " << chunks
                                 << " Chunks from disk.";
    return flag_stream;
}
} // namespace util
} // namespace osrm

#endif // OSRM_INCLUDE_UTIL_IO_HPP_
