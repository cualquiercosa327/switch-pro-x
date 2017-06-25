#include <Windows.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include <cstdint>

#include <ViGEmUM.h>

#include "common.h"
#include "connection_callback.h"
#include "switch-pro-x.h"
#include "ProControllerDevice.h"

namespace {
    std::map<libusb_device *, std::unique_ptr<ProControllerDevice>> proControllers;
    std::mutex controllerMapMutex;
}

void AddController(libusb_device *dev)
{
    std::lock_guard<std::mutex> lk(controllerMapMutex);
    auto device = std::make_unique<ProControllerDevice>(dev);
    if (device->Valid()) {
        std::cout << "FOUND PRO CONTROLLER: " << device.get() << std::endl;
        proControllers[dev] = std::move(device);
    }
}

void RemoveController(libusb_device *dev)
{
    std::lock_guard<std::mutex> lk(controllerMapMutex);
    auto it = proControllers.find(dev);
    if (it != proControllers.end())
    {
        std::cout << "REMOVED PRO CONTROLLER: " << it->second.get() << std::endl;
        proControllers.erase(it);
    }
}

int main()
{
    auto ret = vigem_init();

    if (!VIGEM_SUCCESS(ret))
    {
        std::cerr << "error initializing ViGEm: " << std::hex << std::showbase << ret << std::endl;

        system("pause");

        return 1;
    }

    SetupDeviceNotifications();

    return 0;
}
