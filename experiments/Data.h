#ifndef DATA_H
#define DATA_H

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

class Data {
public:
    static void GetUniqueRandomData(uint64_t numOfKeyValuePairs, std::vector<uint64_t> &data) {
        data.reserve(numOfKeyValuePairs);
        // Fill in the data vector with numbers from 1 to numOfKeyValuePairs.
        std::iota(data.begin(), data.end(), 1);

        // Randomize the values in the data.
        std::random_device rd;
        std::mt19937 pseudo_random_generator(rd());
        std::shuffle(data.begin(), data.end(), pseudo_random_generator);
    }

    static std::vector<uint64_t> GetRandomSkewedData(uint64_t numToGenerate, uint64_t lower, uint64_t upper, int skew) {
        std::vector<uint64_t> data(numToGenerate, 0);

        // Generate random values in the range from fromKey to toKey, generating possibly duplicates.
        std::random_device rd;
        std::mt19937 pseudo_random_generator(rd());
        std::uniform_int_distribution<uint64_t> randomDistribution(lower, upper + skew);
        for (int i = 0; i < numToGenerate; i++) {
            uint64_t num = randomDistribution(pseudo_random_generator);
            if (num <= 2) {
                data[i] = num;
            }
        }
        return data;
    }

    static void RandomizeData(std::vector<uint64_t> &data) {
        // Randomize the values in the data.
        std::random_device rd;
        std::mt19937 pseudo_random_generator(rd());
        std::shuffle(data.begin(), data.end(), pseudo_random_generator);
    }
};

#endif // DATA_H