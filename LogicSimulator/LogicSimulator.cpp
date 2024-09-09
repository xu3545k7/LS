
#include "LogicSimulator.h"

#include "..\Device\AndGate.h"
#include "..\Device\NotGate.h"
#include "..\Device\OrGate.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>

LogicSimulator::~LogicSimulator() {
    for (auto device : circuit) {
        delete device;
    }
    for (auto oPin : oPins) {
        delete oPin;
    }
}
bool LogicSimulator::load(const std::string& filePath) {
    

    
    std::ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    size_t numInputPins, numGates;
    if (!(file >> numInputPins >> numGates)) {
        return false;
    }

    iPins.resize(numInputPins);
    for (size_t i = 0; i < numInputPins; ++i) {
        iPins[i] = new InputPin(i);
    }

    // 清除讀取後剩下的換行符號
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string line;
    for (size_t i = 0; i < numGates; ++i) {
        std::getline(file, line);
        
        // 清理換行和額外空格符號
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        std::stringstream ss(line);
        int gateType;
        ss >> gateType;

        std::vector<std::string> inputs;
        std::string inputId;
        while (ss >> inputId && inputId != "0") {
            inputs.push_back(inputId);
        }

        gatesInfo.push_back({gateType, inputs});
        Device* gate = nullptr;

        switch (gateType) {
            case 1:
                gate = new AndGate({});
                break;
            case 2:
                gate = new OrGate({});
                break;
            case 3:
                gate = new NotGate(nullptr);
                break;
            default:
                return false;
        }

        circuit.push_back(gate);
    }

    for (size_t i = 0; i < gatesInfo.size(); ++i) {
        const std::vector<std::string>& inputs = std::get<1>(gatesInfo[i]);
        std::vector<Device*> resolvedInputs;

        //std::cerr << "Debug: Gate " << i + 1 << " receives inputs from: ";

        for (const auto& inputStr : inputs) {
            if (inputStr.find('.') != std::string::npos) {
                size_t dotPos = inputStr.find('.');
                std::string gatePart = inputStr.substr(0, dotPos);
                int gateId = std::stoi(gatePart);
                if (gateId < 1 || gateId > static_cast<int>(circuit.size())) {
                    //std::cerr << "Debug: Invalid gate ID in input: " << gateId << std::endl;
                    return false;
                }
                resolvedInputs.push_back(circuit[gateId - 1]);
                usedOutputs.insert(gateId - 1);  // 標記該gate的輸出已被使用
                //std::cerr << "Gate " << gateId;
            } else {
                int inputId = std::stoi(inputStr);
                if (inputId < 0) {
                    // 連接到輸入針腳
                    if (static_cast<size_t>(-inputId - 1) >= iPins.size()) {
                        //std::cerr << "Debug: Invalid input pin ID: " << inputId << std::endl;
                        return false;
                    }
                    resolvedInputs.push_back(iPins[-inputId - 1]);
                    //std::cerr << "InputPin " << -inputId;
                } else {
                    // 連接到其他閘門
                    if (inputId < 1 || inputId > static_cast<int>(circuit.size())) {
                        //std::cerr << "Debug: Invalid gate ID in input: " << inputId << std::endl;
                        return false;
                    }

                    // 檢查是否連結到自己
                    if (inputId - 1 == int(i)) {
                        std::cout << "Error: Logic gate cannot connect to itself." << std::endl;
                        
                        return false;  // 返回 false 以結束 load
                    }

                    resolvedInputs.push_back(circuit[inputId - 1]);
                    usedOutputs.insert(inputId - 1);  // 標記該閘門的輸出已被使用
                    //std::cerr << "Gate " << inputId;
                }
            }
            //std::cerr << ", ";
        }

        //std::cerr << std::endl;

        switch (std::get<0>(gatesInfo[i])) {
            case 1:
                dynamic_cast<AndGate*>(circuit[i])->setInputs(resolvedInputs);
                //std::cerr << "Debug: AND gate " << i + 1 << " linked with " << resolvedInputs.size() << " inputs." << std::endl;
                break;
            case 2:
                dynamic_cast<OrGate*>(circuit[i])->setInputs(resolvedInputs);
                //std::cerr << "Debug: OR gate " << i + 1 << " linked with " << resolvedInputs.size() << " inputs." << std::endl;
                break;
            case 3:
                if (resolvedInputs.empty()) {
                    //std::cerr << "Debug: NOT gate " << i + 1 << " has no inputs." << std::endl;
                    return false;
                }
                dynamic_cast<NotGate*>(circuit[i])->setInputs(resolvedInputs[0]);
                //std::cerr << "Debug: NOT gate " << i + 1 << " linked with 1 input." << std::endl;
                break;
            default:
                //std::cerr << "Debug: Invalid gate type encountered during linking." << std::endl;
                break;
        }
    }
    
    // 識別最終輸出閘門：那些輸出未被其他閘門使用的閘門
    for (size_t i = 0; i < circuit.size(); ++i) {
        if (!usedOutputs.count(i)) {
            OutputPin* oPin = new OutputPin({circuit[i]});
            oPins.push_back(oPin);
            //std::cerr << "Debug: Gate " << i + 1 << " is an output gate and added to oPins." << std::endl;
        }
    }
    std::cerr << "Circuit: " << numInputPins << " input pins, " << oPins.size() << " output pins and " << numGates << " gates" << std::endl;
    return true;
}

std::string LogicSimulator::getSimulationResult(const std::vector<bool>& inputValues) {
    if (inputValues.size() != iPins.size()) {
        return "Error: Number of input values does not match number of input pins.";
    }
    std::stringstream result;
    for (size_t i = 0; i < iPins.size(); ++i) {
        iPins[i]->setValue(inputValues[i]);
        result << inputValues[i] << " ";
    }

    
    for (auto& oPin : oPins) {
        result << "| " << oPin->getOutput() << " ";
    }
    return result.str();
}

std::string LogicSimulator::getTruthTable() {
    std::stringstream result;
    size_t numInputs = iPins.size();
    size_t numRows = 1 << numInputs;

    for (size_t i = 0; i < numRows; ++i) {
        std::vector<bool> inputs(numInputs);
        for (size_t j = 0; j < numInputs; ++j) {
            inputs[j] = (i >> (numInputs - j - 1)) & 1;
        }

        
        result << getSimulationResult(inputs) << std::endl;
    }

    return result.str();
}

void LogicSimulator::addInputPin(Device* iPin) {
    iPins.push_back(static_cast<InputPin*>(iPin));
}

size_t LogicSimulator::getInputPinCount() const {
    return iPins.size();
}

void LogicSimulator::clearGates(){
    // 清除之前的電路和引腳資料
    for (auto device : circuit) {
        delete device;
    }
    circuit.clear();
    gatesInfo.clear();
    usedOutputs.clear();

    for (auto inputPin : iPins) {
        delete inputPin;
    }
    iPins.clear();

    for (auto outputPin : oPins) {
        delete outputPin;
    }
    oPins.clear();

    std::cerr << "All gates, input pins, and output pins have been cleared." << std::endl;
    std::cerr << "Current circuit size: " << circuit.size() << std::endl;
    std::cerr << "Current input pins size: " << iPins.size() << std::endl;
    std::cerr << "Current output pins size: " << oPins.size() << std::endl;
}
