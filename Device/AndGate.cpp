#include "AndGate.h"

// �c�y��ơA������J�]�ƪ��V�q
AndGate::AndGate(const std::vector<Device*>& in) : inputs(in), outputs() {
    this->inputs = in;
    this->outputs = {};

    if (outputs.empty()) {
        outputs.push_back(nullptr); 
    }
}


AndGate::~AndGate() {}

// ������AND������X��
bool AndGate::getOutput()  {
    bool result = true;
    for (auto& input : inputs) {
        result = result && input->getOutput() ;
    }
    return result;
}

// �K�[��J�޸}
void AndGate::addInputPins(const std::vector<Device*>& devices) {
    for (Device* device : devices) {
        inputs.push_back(device);
    }
}

// �K�[��X�޸}
void AndGate::addOutputPin(Device* output) {
    outputs.push_back(output);
}

// �]�m��J�޸}
void AndGate::setInputs(const std::vector<Device*>& inputs) {
    this->inputs = inputs;
}
