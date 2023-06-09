#ifndef CSC443_PROJECT_UTILS_H
#define CSC443_PROJECT_UTILS_H

#include <cstdint>
#include <vector>
#include <cstddef>
#include <bitset>
#include <limits>

using DataEntry_t = std::pair<uint64_t, uint64_t>;

namespace Utils {
    const uint64_t INVALID_VALUE = std::numeric_limits<uint64_t>::max();
    const uint64_t DELETED_KEY_VALUE = std::numeric_limits<uint64_t>::max() - 1; // Used in LSMTree
    const uint64_t BYTE_SIZE = 8;
    const uint64_t EIGHT_BYTE_SIZE = 64;
    const std::string SST_FILE_EXTENSION = ".sst";
    const std::string LEVEL = "level";

    /**
     * Search for the given key within the given vector.
     *
     * @param keys vector to search through
     * @param key the key to find
     * @return index of the key if found.
     */
    int BinarySearch(std::vector<uint64_t> keys, uint64_t key, int startIndex = 0);

    /**
     * Converts an integer to its binary form and taking <numBits> number
     * of least significant bits.
     *
     * @param integer integer to get binary string of.
     * @param numBits the number of LSB bits.
     * @return numBits number of LSB of the binary string of the given integer.
     */
    std::string GetBinaryFromInt(uint64_t integer, int numBits);

    /**
     * Returns a fileName with ".sst" extension added.
     *
     * @param fileName filename to add SST extension to.
     */
    std::string GetFilenameWithExt(const std::string &fileName);

    /**
     * Extract the keys from the key-value pairs in the input data.
     *
     * @param data
     * @returns the list of keys in the input data.
     */
    std::vector<uint64_t> GetKeys(std::vector<uint64_t> &data);

    /**
     * Opens the file at given filename. Prints out error if the operation fails.
     *
     * @param fileName the file to open
     * @return the file descriptor created by opening the file.
     */
    int OpenFile(const std::string& fileName);

    /**
     * Ensures the given directory has a slash at the end.
     *
     * @param directory the directory name
     * @return the directory name with a slash at the end.
     */
    std::string EnsureDirSlash(std::string directory);
}

#endif //CSC443_PROJECT_UTILS_H
