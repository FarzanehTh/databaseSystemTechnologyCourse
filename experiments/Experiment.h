#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "Db.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;
namespace chrono = std::chrono;

enum Operation {
    Put = 0, Get = 1, Scan = 2
};

const int ONE_MEGA_BYTE = 1024 * 1024;
const int ONE_GIGA_BYTE = 1024 * 1024 * 1024;
const std::string EXPERIMENT_DB_PATH = "experiments_db"; // To contain the SST files
const std::string EXPERIMENT_CSV_PATH = "experiments_db_CSV"; // To contain CSV files
// Each input element to db is a (key, value) pair of types uint64_t, which has the size of 16 bytes.
const int KV_BYTE_SIZE = sizeof(uint64_t) * 2;
const std::string EVICTION_POLICIES_NAMES[2] = {"LRU", "CLOCK"};
const std::string SEARCH_TYPES_NAMES[2] = {"BinarySearch", "BTreeSearch"};

class Experiment {
    Db *db;
    std::vector<uint64_t> data;

    std::string outputDir;
    uint64_t numKVPairs;
    int memtableSize;
    int bufferMaxSize;
    EvictionPolicyType evictionPolicy;
    SearchType searchType;
    int bloomFilterBits;

    /**
     * Write experiment data in CSV format to file at given path.
     *
     * @param filename the file path to the CSV file.
     * @param elapsedTime the amount of time the experiment took.
     * @param throughput the throughput of the experiment.
     * @param inputByteSize the input size of data in byte.
     */
    void WriteDataToFile(const std::string &filename, double elapsedTime, double throughput,
                         uint64_t inputByteSize = 0) const {
        // Write CSV file header for new file
        bool fileIsNew = !fs::exists(this->outputDir + filename);
        std::ofstream outputFile(this->outputDir + filename, std::ofstream::out | std::ofstream::app);
        if (fileIsNew) {
            outputFile << "dbMemtableMaxSize(MB)" << ","
                       << "bufferPoolMaxSize" << ","
                       << "evictionPolicy" << ","
                       << "searchType" << ","
                       << "inputDataSize(MB)" << ","
                       << "elapsedTime(sec)" << ","
                       << "throughput(MB/sec)" << ","
                       << "latency(sec)" << ","
                       << "bloomFilterBits"
                       << std::endl;
        }

        uint64_t inputDataSize = (inputByteSize) ? inputByteSize : this->numKVPairs * KV_BYTE_SIZE;
        outputFile << this->memtableSize / ONE_MEGA_BYTE << ","
                   << this->bufferMaxSize << ","
                   << EVICTION_POLICIES_NAMES[this->evictionPolicy] << ","
                   << SEARCH_TYPES_NAMES[this->searchType] << ","
                   << inputDataSize / ONE_MEGA_BYTE << ","
                   << elapsedTime << ","
                   << throughput << "," // throughput
                   << elapsedTime / this->numKVPairs << "," // latency
                   << this->bloomFilterBits
                   << std::endl;

        outputFile.close();
    }

    /**
     * Runs the Put operation with keys from randomly generated data.
     *
     * @return the amount of data processed (measured in # of keys).
    */
    uint64_t RunPutOperation(std::vector<uint64_t> &operations) {
        for (int i = 0; i < this->data.size(); i++) {
            switch (operations.at(i)) {
                case 0:
                    this->db->Put(i, i * 10);
                    break;
                case 1:
                    this->db->Update(i, i * 5);
                    break;
                case 2:
                    this->db->Delete(i);
                    break;
            }

        }
        return this->data.size();
    }

    /**
     * Runs the Get operation using keys of within range [1, inputSize].
     *
     * @param numQueries the number of GET queries to run.
     * @return the amount of data processed (measured in # of keys).
    */
    uint64_t RunGetOperation(uint64_t numQueries = 0) {
        for (int i = 0; i < numQueries; i += 1) {
            this->db->Get(this->data.at(i));
        }
        return numQueries;
    }

    /**
     * Runs the Scan db operation on the input db and uses the range [1, inputSize]
     * as range of keys to scan.
     *
     * @param inputSize the amount of data to scan.
     * @return the amount of data processed (measured in # of keys).
     */
    uint64_t RunScanOperation(uint64_t inputSize = 0) {
        uint64_t key2 = (inputSize) ? inputSize : this->numKVPairs;
        std::vector<DataEntry_t> scanResult;
        this->db->Scan(1, key2, scanResult);
        return scanResult.size();
    }

public:
    explicit Experiment(std::string outputDir, uint64_t dataByteSize, int memtableSize, SearchType searchType,
                        int bloomFilterBits = 0) {
        this->outputDir = Utils::EnsureDirSlash(std::move(outputDir));
        this->numKVPairs = dataByteSize / KV_BYTE_SIZE;
        this->memtableSize = memtableSize;
        this->searchType = searchType;
        this->bufferMaxSize = 0;
        this->evictionPolicy = EvictionPolicyType::LRU_t;
        this->bloomFilterBits = bloomFilterBits;

        // Create new db using given parameter, exit if fails
        int memtableKVPairs = this->memtableSize / KV_BYTE_SIZE;
        if (this->bloomFilterBits == 0) {
            this->db = new Db(memtableKVPairs, searchType);
        } else {
            auto bufferPool = new BufferPool(pow(2, 3), pow(2, 8), LRU_t);
            auto lsmTree = new LSMTree(bloomFilterBits, 8, 8);
            this->db = new Db(memtableKVPairs, searchType, bufferPool, lsmTree);
        }
        if (!this->db->Open(EXPERIMENT_DB_PATH)) {
            std::cout << "Failed to open DB at path " << EXPERIMENT_DB_PATH << std::endl;
            exit(1);
        }

        // Generate random data
        std::vector<uint64_t> dataVec(this->numKVPairs);
        this->data = dataVec;
        std::cout << "Generate random DB data " << this->numKVPairs << "\n";
        Data::GetUniqueRandomData(this->numKVPairs, this->data);
    }

    ~Experiment() {
        delete db;
        Experiment::ResetDbDirectory();
    }

    /**
     * Resets the database directory by removing all files within it.
     */
    static void ResetDbDirectory() {
        std::filesystem::remove_all(EXPERIMENT_DB_PATH);
    }

    /**
     * Randomize the order of the data used in current experiment.
     */
    void RandomizeData() {
        Data::RandomizeData(this->data);
    }

    /**
     * Sort the data used in current experiment in ascending order.
     */
    void SortData() {
        std::sort(this->data.begin(), this->data.end());
    }

    /**
     * Reset database buffer pool with new buffer pool max size and eviction policy.
     *
     * The minimum buffer pool size is fixed at 2^3 = 8.
     *
     * @param newMaxSize the new max size for the buffer pool.
     * @param newEvictionPolicy the new eviction policy for the buffer pool.
     */
    void ResetBufferPool(int newMaxSize, EvictionPolicyType newEvictionPolicy) {
        int bufferMinSize = pow(2, 3);
        this->bufferMaxSize = newMaxSize;
        this->evictionPolicy = newEvictionPolicy;
        this->db->ResetBufferPool(bufferMinSize, newMaxSize, newEvictionPolicy);
    }

    /**
     * Inserts generated experiment data into the database if the database is empty.
     */
    void InsertDataIntoDb() {
        std::vector<uint64_t> operations(this->data.size(), 0);
        RunPutOperation(operations);
    }

    /**
     * Runs experiment of given operation and input data size.
     *
     * @param operation the operation the experiment will execute.
     * @param outputFilename the file path to the output CSV file.
     * @param inputByteSize the byte size of input data.
     */
    void RunExperiment(Operation operation, const std::string &outputFilename, uint64_t inputByteSize = 0) {
        uint64_t dataByteSize = (inputByteSize) ? inputByteSize : this->numKVPairs * KV_BYTE_SIZE;

        std::cout << "Input: " << dataByteSize / ONE_MEGA_BYTE << " | "
                  << "Op: " << operation << " | "
                  << "Evict: " << EVICTION_POLICIES_NAMES[this->evictionPolicy] << " | "
                  << "Search: " << SEARCH_TYPES_NAMES[this->searchType] << " | "
                  << "Buffer: " << this->bufferMaxSize << " | "
                  << "Memtable: " << this->memtableSize << " | "
                  << "BF: " << this->bloomFilterBits << "\n";

        // Run experiment
        uint64_t dataProcessed;
        chrono::time_point<chrono::system_clock> start;
        switch (operation) {
            case Operation::Put: {
                std::vector<uint64_t> operations(this->data.size(), 0);
                if (this->bloomFilterBits > 0) {
                    // Add in skew factor to generate more PUT ops than UPDATE and DELETE
                    operations = Data::GetRandomSkewedData(this->data.size(), 0, 2, 5);
                }

                start = chrono::high_resolution_clock::now();
                dataProcessed = this->RunPutOperation(operations);
                break;
            }
            case Operation::Get: {
                // The number of queries is limited to 4 if data size is bigger than memtable size
                // per SST to reduce overall runtime.
                uint64_t numSstFiles = ceil((double) dataByteSize / this->memtableSize);
                uint64_t numQueries = dataByteSize > this->memtableSize ? 4 * numSstFiles : dataByteSize / KV_BYTE_SIZE;

                start = chrono::high_resolution_clock::now();
                dataProcessed = this->RunGetOperation(numQueries);
                break;
            }
            case Operation::Scan: {
                uint64_t inputSize = dataByteSize / KV_BYTE_SIZE;
                if (this->bloomFilterBits > 0) {
                    inputSize /= 1024;
                }

                start = chrono::high_resolution_clock::now();
                dataProcessed = this->RunScanOperation(inputSize);
                break;
            }
        }
        auto end = chrono::high_resolution_clock::now();

        // Calculate experiment statistics and write to file
        chrono::duration<double> elapsedTime = end - start;
        double throughput = (dataProcessed * KV_BYTE_SIZE) / (ONE_MEGA_BYTE * elapsedTime.count() * 1.0);
        std::cout << "Data processed: " << dataProcessed << "\n";

        // Write data to file
        this->WriteDataToFile(outputFilename, elapsedTime.count(), throughput, inputByteSize);
    }
};

#endif // EXPERIMENTS_H