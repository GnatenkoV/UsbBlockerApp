//
//  UsbBlockerDriver.cpp
//  UsbBlockerDriver
//
//  Created by user on 20.11.2024.
//

#include "pch.h"
#include <DriverKit/IOUserServer.h>
#include <DriverKit/IOLib.h>
#include <DriverKit/OSDictionary.h>
#include <DriverKit/OSNumber.h>
#include <USBDriverKit/USBDriverKit.h>

#include "UsbBlockerDriver.h"
#include "UsbBlockerUserClient.h"

struct UsbBlockerDriver_IVars {
    IOUSBHostDevice *usbDevice;
    uint8_t configValue;
};

// MARK: Dext Lifecycle Management
bool UsbBlockerDriver::init(void)
{
    bool result = false;

    LogI("init");

    result = super::init();
    if (result != true)
    {
        LogE("init - super::init failed.");
        goto Exit;
    }

    ivars = IONewZero(UsbBlockerDriver_IVars, 1);
    if (ivars == nullptr)
    {
        LogE("init - Failed to allocate memory for ivars.");
        goto Exit;
    }

    LogI("init - Finished.");
    return true;

Exit:
    return false;
}

kern_return_t
IMPL(UsbBlockerDriver, Start)
{
    LogI("Start");
    kern_return_t ret;
    ret = Start(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        LogE("Super start failed with error: %x", ret);
        return ret;
    }
    
    LogI("Super start succeeded");
    ivars->usbDevice = OSRequiredCast(IOUSBHostDevice, provider);
    if (ivars->usbDevice == nullptr)
    {
        LogE("Failed to get IOUSBHostDevice instance.");
        return kIOReturnNoDevice;
    }
    
    LogI("USB device object cast succeeded");
    ObtainConfigurationValue();
    LogI("Configuration value obtained");
    ConfigureUSBDevice(true);
    LogI("Device configured");
    
    ret = RegisterService();
    if (ret != kIOReturnSuccess)
    {
        LogE("Register service failed with error: %x", ret);
        return ret;
    }
    
    LogI("Driver is configured and registered successfully.");
    
    return ret;
}

kern_return_t
IMPL(UsbBlockerDriver, Stop)
{
    kern_return_t ret = kIOReturnSuccess;
    LogI("Stop()");
    
    ret = Stop(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        LogE("Stop() - super::Stop failed with error: 0x%08x.", ret);
        return ret;
    }

    LogI("Stop() - Finished.");

    return ret;
}

void UsbBlockerDriver::free(void)
{
    LogI("free()");

    OSSafeReleaseNULL(ivars->usbDevice);
    IOSafeDeleteNULL(ivars, UsbBlockerDriver_IVars, 1);

    super::free();
}

// When an application attaches to the dext via IOServiceOpen, this method runs as a callback.
kern_return_t
IMPL(UsbBlockerDriver, NewUserClient)
{
    kern_return_t ret = kIOReturnSuccess;
    IOService* client = nullptr;
    LogI("NewUserClient()");

    ret = Create(this, "UserClientProperties", &client);
    if (ret != kIOReturnSuccess)
    {
        LogE("NewUserClient() - Failed to create UserClientProperties with error: 0x%08x.", ret);
        return ret;
    }

    *userClient = OSDynamicCast(UsbBlockerUserClient, client);
    if (*userClient == NULL)
    {
        LogE("NewUserClient() - Failed to cast new client.");
        client->release();
        return ret;
    }
    
    UsbBlockerUserClient* dextClient = OSRequiredCast(UsbBlockerUserClient, client);
    dextClient->SetParent(this);

    LogI("NewUserClient() - Finished.");
    return ret;
}

void UsbBlockerDriver::ObtainConfigurationValue()
{
    Log("ObtainConfigurationValue started");
    ivars->configValue = 1;
    OSContainer* config;
    kern_return_t ret = ivars->usbDevice->SearchProperty(kUSBHostDevicePropertyNumConfigs, "", 0, &config);
    if (ret != kIOReturnSuccess)
    {
        Log("Failed to obtain number of configurations. Result - %d; 0x%08x", ret, ret);
        OSSafeReleaseNULL(config);
        return;
    }

    OSNumber* configNumber = OSRequiredCast(OSNumber, config);
    Log("Obtained kUSBHostDevicePropertyNumConfigs - %d", configNumber->unsigned8BitValue());
    if (configNumber->unsigned8BitValue() == 0)
    {
        Log("Obtained wrong number of configurations. Use default.");
        OSSafeReleaseNULL(config);
        return;
    }
    
    int index = 0;
    const IOUSBConfigurationDescriptor* configDesc = ivars->usbDevice->CopyConfigurationDescriptor(index);
    if (configDesc == nullptr)
    {
        Log("Failed to get configuration descriptor. Use default.");
        OSSafeReleaseNULL(config);
        return;
    }
    
    if (configDesc->bConfigurationValue != 0)
    {
        ivars->configValue = configDesc->bConfigurationValue;
        Log("Obtained bConfigurationValue - %d", configDesc->bConfigurationValue);
    }
    else
    {
        Log("Obtained bConfigurationValue is equal to 0. Use default.");
    }
    
    OSSafeReleaseNULL(config);
    Log("ObtainConfigurationValue finished");
}

bool UsbBlockerDriver::ConfigureUSBDevice(bool enable)
{
    Log("ConfigureUSBDevice called");
    if (ivars->usbDevice == nullptr)
    {
        Log("Failed to change IOUSBHostDevice state the ivars->usbDevice is nullptr.");
        return false;
    }
    
    kern_return_t ret = ivars->usbDevice->Open(this, 0, NULL);
    if (ret != kIOReturnSuccess)
    {
        Log("Failed to open connection to usbDevice object. result - %d; 0x%08x", ret, ret);
        return false;
    }
    
    ret = ivars->usbDevice->SetConfiguration((enable ? ivars->configValue : 0), enable);
    if (ret != kIOReturnSuccess)
    {
        Log("Failed to SetConfiguration for usbDevice object. result - %d; 0x%08x", ret, ret);
        return false;
    }
    
    ret = ivars->usbDevice->Close(this, 0);
    if (ret != kIOReturnSuccess)
    {
        Log("Failed to close connection to usbDevice object. result - %d; 0x%08x", ret, ret);
        return false;
    }
    
    Log("ConfigureUSBDevice finished");
    return true;
}
