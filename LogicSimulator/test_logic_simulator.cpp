#include <gtest/gtest.h>

#include "LogicSimulator.h"

// ���� getInputPinCount ���
TEST(LogicSimulatorTest, GetInputPinCount) {
    LogicSimulator simulator;
    
    simulator.clearGates();
    simulator.load("file1.lcf");

    EXPECT_EQ(simulator.getInputPinCount(), 3);
}

// ���� getSimulationResult ���
TEST(LogicSimulatorTest, GetSimulationResult) {
    LogicSimulator simulator;

    simulator.clearGates();
    simulator.load("file1.lcf");

    std::vector<bool> inputs = {1, 0, 1}; 
    std::string result = simulator.getSimulationResult(inputs);
    EXPECT_EQ(result, "1 0 1 | 1 ");
}
