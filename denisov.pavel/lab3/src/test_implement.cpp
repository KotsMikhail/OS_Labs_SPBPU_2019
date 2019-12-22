#include <iostream>
#include <new>

#include "test.h"
#include "test_utils.h"

#include "fine_grained_set.h"
#include "lazy_synchronization_set.h"

#include "simple_data_set_creator.h"
#include "fixed_data_set_creator.h"
#include "random_data_set_creator.h"

#include "writers_test.h"
#include "readers_test.h"
#include "general_test.h"


static std::string _getSetName (const TESTING_SETS &setType)
{
    return setType == TESTING_SETS::FINE_GRAINED ? "\"Fine-Grained\"" : "\"Lazy-Synchronization\"";
}


static std::string _getTestName (const AVAILABLE_TEST_RUNNER_ID &id)
{
    switch (id) {
        case AVAILABLE_TEST_RUNNER_ID::WRITERS:
            return "Writing test";
        case AVAILABLE_TEST_RUNNER_ID::READERS:
            return "Reading test";
        case AVAILABLE_TEST_RUNNER_ID::GENERAL:
            return "General test";
        default:
            return "";
    }
}


static std::string _getDataSetCreatorName (const AVAILABLE_DATA_SET_CREATOR_ID &id)
{
    switch (id) {
        case AVAILABLE_DATA_SET_CREATOR_ID::SIMPLE:
            return "Simple";
        case AVAILABLE_DATA_SET_CREATOR_ID::RANDOM:
            return "Random";
        case AVAILABLE_DATA_SET_CREATOR_ID::FIXED:
            return "Fixed";
        default:
            return "";
    }
}


template<typename ELEMENT_TYPE>
static SET<ELEMENT_TYPE> * _createSet (const TESTING_SETS &setType)
{
    switch (setType) {
        case TESTING_SETS::FINE_GRAINED:
            return SET_FINE_GRAINED<ELEMENT_TYPE>::Create();
        case TESTING_SETS::LAZY_SYNCHRONIZATION:
            return SET_LAZY_SYNCHRONIZATION<ELEMENT_TYPE>::Create();
        default:
            return nullptr;
    }
}


static TEST_RUNNER * _createTestRunner (const TEST_TYPE &testType)
{
    switch (testType) {
        case TEST_TYPE::WRITERS_TEST:
            return new (std::nothrow) WRITERS_TEST_RUNNER();
        case TEST_TYPE::READERS_TEST:
            return new (std::nothrow) READERS_TEST_RUNNER();
        case TEST_TYPE::GENERAL_TEST:
            return new (std::nothrow) GENERAL_TEST_RUNNER();
        default:
            return nullptr;
    }
}


void runTest (const TESTING_SETS &setType, const TEST_TYPE &testType)
{
    SET<int> *set = _createSet<int>(setType);
    TEST_RUNNER *testRunner = _createTestRunner(testType);
    SIMPLE_DATA_SET_CREATOR *simpleDSCreator = new (std::nothrow) SIMPLE_DATA_SET_CREATOR();

    if (set && testRunner) {
        if (testRunner->Run(set, simpleDSCreator, true)) {
            std::cout << _getSetName(setType) << "; " << _getTestName(testRunner->id) << " succeeded." << std::endl;
        } else {
            std::cout << _getSetName(setType) << "; " << _getTestName(testRunner->id) << " failed." << std::endl;
        }
    }

    delete set;
    delete testRunner;
    delete simpleDSCreator;
}


void runTimeTest (void)
{
    TEST_RUNNER * testRunners[] = {new (std::nothrow) WRITERS_TEST_RUNNER(), new (std::nothrow) READERS_TEST_RUNNER(), new (std::nothrow) GENERAL_TEST_RUNNER()};
    DATA_SET_CREATOR * dsCreators[] = {new (std::nothrow) FIXED_DATA_SET_CREATOR(), new (std::nothrow) RANDOM_DATA_SET_CREATOR()};
    TESTING_SETS setTypes[] = {TESTING_SETS::FINE_GRAINED, TESTING_SETS::LAZY_SYNCHRONIZATION};
    double time;
    char line[256];

    std::cout << "------------------------------------------------------------------------------" << std::endl;
    std::cout << "NAME                                         | TIME          " << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;
    for (TEST_RUNNER * testRunner : testRunners) {
        for (TESTING_SETS setType : setTypes) {
            for (DATA_SET_CREATOR * dsCreator : dsCreators) {
                sprintf(line, "%22s %12s (%6s) | ", _getSetName(setType).c_str(), _getTestName(testRunner->id).c_str(), _getDataSetCreatorName(dsCreator->id).c_str());

                SET<int> *set = _createSet<int>(setType);
                if (testRunner->RunWithTimeMeasure(set, dsCreator, true, &time)) {
                    sprintf(line + 47, "%8s", std::to_string(time).c_str());
                } else {
                    sprintf(line + 47, "NA");
                }

                std::cout << line << std::endl;
            }
            std::cout << "------------------------------------------------------------------------------" << std::endl;
        }
    }

    for (TEST_RUNNER * testRunner : testRunners) {
        delete testRunner;
    }

    for (DATA_SET_CREATOR * dataSetCreator : dsCreators) {
        delete dataSetCreator;
    }
}
