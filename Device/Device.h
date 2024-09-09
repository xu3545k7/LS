#ifndef DEVICE_H
#define DEVICE_H

#include <vector>

class Device {
public:
    virtual bool getOutput() = 0;  // �µ����禡
    virtual void addInputPins(const std::vector<Device*>& devices) = 0;  // �s�W�µ����禡
    virtual void addOutputPin(Device* output) = 0; // �s�W�µ����禡
    virtual ~Device() = default;
};

#endif
