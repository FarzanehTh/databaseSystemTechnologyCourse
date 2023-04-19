#include "Db.h"
#include "Data.h"
#include "Experiment.h"
#include <string>

void RunExperimentsStepOne() {
    std::cout << "Running experiment Step 1\n";

    const std::string outputDir = EXPERIMENT_CSV_PATH + "_step1";
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    for (int dbMemtableSize: {ONE_MEGA_BYTE, 4 * ONE_MEGA_BYTE}) {

        // Vary the data size up to 1GB.
        uint64_t inputDataByteSize = ONE_MEGA_BYTE;
        while (inputDataByteSize <= ONE_GIGA_BYTE) {
            // Clear experiment db directory
            Experiment::ResetDbDirectory();

            // Run the experiments.
            auto experiment = Experiment(outputDir, inputDataByteSize, dbMemtableSize, SearchType::BINARY_SEARCH);

            experiment.RunExperiment(Operation::Put, "put_operation.csv");
            // Randomize the input data before Get queries.
            experiment.RandomizeData();
            experiment.RunExperiment(Operation::Get, "get_operation.csv");
            experiment.RunExperiment(Operation::Scan, "scan_operation.csv");

            // Update experiment parameters
            inputDataByteSize <<= 1;
        }
    }
}

void EvictionPolicyExperiment(const std::string &outputDir, bool randomizeData) {
    // Clear experiment db directory
    Experiment::ResetDbDirectory();

    // Prepare the db and its data SST files.
    int memtableByteSize = ONE_MEGA_BYTE;
    auto experiment = Experiment(outputDir, ONE_GIGA_BYTE, memtableByteSize, SearchType::BINARY_SEARCH);

    // Insert data into DB first
    experiment.InsertDataIntoDb();

    // Randomize/Sort data before running queries
    randomizeData ? experiment.RandomizeData() : experiment.SortData();

    for (auto evictionPolicy: {EvictionPolicyType::LRU_t, EvictionPolicyType::CLOCK_t}) {
        // Run the "Get" operation on db with different buffer pool max sizes.
        // Vary the max size of the buffer up to half a GB.
        int bufferPoolMaxSize = pow(2, 8);
        while (bufferPoolMaxSize <= pow(2, 17)) {
            // Reset buffer pool before each run
            experiment.ResetBufferPool(bufferPoolMaxSize, evictionPolicy);

            // Perform "Get" queries on the same data with the same spatial locality.
            // LRU performs better if the queries have spatial locality.
            std::string experimentType = (randomizeData) ? "random" : "spatial";
            experiment.RunExperiment(Operation::Get, "get_operation_eviction_" + experimentType + ".csv");

            // Update experiment parameters
            bufferPoolMaxSize <<= 2;
        }
    }
}

void BinarySearchBTreeExperiment(const std::string &outputDir) {
    // Let the memtable's byte size, the min and max size of the buffer pool,
    // and the eviction policy be fixed. Vary the data size
    // (for the "Get" and "Scan" operations) and search type.
    int memtableByteSize = ONE_MEGA_BYTE;

    // Run experiment
    for (auto &searchType: {SearchType::BINARY_SEARCH, SearchType::B_TREE_SEARCH}) {
        // Clear experiment db directory
        Experiment::ResetDbDirectory();

        auto experiment = Experiment(outputDir, ONE_GIGA_BYTE, memtableByteSize, searchType);
        experiment.InsertDataIntoDb();

        // Randomize the input data before Get queries.
        experiment.RandomizeData();

        // Run "Get" and "Scan" operations on this db instance by varying the data size.
        uint64_t inputByteSize = 2 * memtableByteSize;
        while (inputByteSize <= ONE_GIGA_BYTE) {
            experiment.ResetBufferPool(pow(2, 8), EvictionPolicyType::LRU_t);
            experiment.RunExperiment(Operation::Get, "get_operation_search_types.csv", inputByteSize);
            experiment.RunExperiment(Operation::Scan, "scan_operation_search_types.csv", inputByteSize);

            // Update experiment parameters
            inputByteSize <<= 1;
        }
    }
}

void RunExperimentsStepTwo() {
    std::cout << "Running experiment Step 2\n";

    const std::string outputDir = EXPERIMENT_CSV_PATH + "_step2";
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    /*** Experiment #1: Compare the efficiency of LRU vs clock by measuring query throughput. ***/
    // Compare eviction policies with data that has spatial locality
    EvictionPolicyExperiment(outputDir, false);
    // Compare eviction policies with random data
    EvictionPolicyExperiment(outputDir, true);

    /*** Experiment #2: Compare the efficiency of BTree and Binary search by measuring query throughput. ***/
    BinarySearchBTreeExperiment(outputDir);
}

void LSMTreeExperiment(const std::string &outputDir) {
    int memtableSize = ONE_MEGA_BYTE;
    uint64_t inputDataByteSize = 2 * memtableSize;
    int bloomFilterNumBits = 5;
    while (inputDataByteSize <= ONE_GIGA_BYTE) {
        // Clear experiment db directory
        Experiment::ResetDbDirectory();

        // Run the experiments.
        auto experiment = Experiment(outputDir, inputDataByteSize, memtableSize, B_TREE_SEARCH, bloomFilterNumBits);

        experiment.RunExperiment(Operation::Put, "put_operation_lsm.csv");

        // Randomize the input data and reset buffer pool before Get queries
        experiment.RandomizeData();
        experiment.ResetBufferPool(pow(2, 8), EvictionPolicyType::LRU_t);

        experiment.RunExperiment(Operation::Get, "get_operation_lsm.csv");
        experiment.RunExperiment(Operation::Scan, "scan_operation_lsm.csv");

        // Update experiment parameters
        inputDataByteSize <<= 1;
    }
}

void BloomFilterExperiment(const std::string &outputDir) {
    int memtableSize = ONE_MEGA_BYTE;
    uint64_t inputDataByteSize = 2 * memtableSize;
    int bloomFilterNumBits = 2;
    while (inputDataByteSize <= ONE_GIGA_BYTE) {
        // Clear experiment db directory
        Experiment::ResetDbDirectory();

        // Run the experiments.
        auto experiment = Experiment(outputDir, inputDataByteSize, memtableSize, B_TREE_SEARCH, bloomFilterNumBits);
        experiment.InsertDataIntoDb();

        // Randomize the input data and reset buffer pool before Get queries
        experiment.RandomizeData();
        experiment.ResetBufferPool(pow(2, 8), EvictionPolicyType::LRU_t);

        experiment.RunExperiment(Operation::Get, "get_operation_bf.csv");

        // Update experiment parameters
        inputDataByteSize <<= 1;
        bloomFilterNumBits++;
    }
}

void RunExperimentStepThree() {
    std::cout << "Running experiment Step 3\n";

    const std::string outputDir = EXPERIMENT_CSV_PATH + "_step3";
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    /** Experiment #1: Measure throughput for PUT, GET, SCAN for varying sizes of data **/
    LSMTreeExperiment(outputDir);

    /** Experiment #2: Measure GET throughput for varying number of bits for bloom filter **/
    BloomFilterExperiment(outputDir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        RunExperimentsStepOne();
        RunExperimentsStepTwo();
        RunExperimentStepThree();
        return 0;
    }

    switch (std::stoi(argv[1])) {
        case 1:
            RunExperimentsStepOne();
            break;
        case 2:
            RunExperimentsStepTwo();
            break;
        case 3:
            RunExperimentStepThree();
            break;
        default:
            std::cout << "Unknown step number!\n";
    }
}
