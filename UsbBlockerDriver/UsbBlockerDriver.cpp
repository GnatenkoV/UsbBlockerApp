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
    uint16_t vendorID;
    uint16_t productID;
};

// MARK: Dext Lifecycle Management
bool UsbBlockerDriver::init(void)
{
    bool result = false;
    //OS_LOG_STRING(<#_ns#>, <#_var#>, <#_str#>)
    LogI("Called");

    result = super::init();
    if (result != true)
    {
        LogE("super::init failed.");
        goto Exit;
    }

    ivars = IONewZero(UsbBlockerDriver_IVars, 1);
    if (ivars == nullptr)
    {
        LogE("Failed to allocate memory for ivars.");
        goto Exit;
    }

    LogI("Finished");
    
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
    LogI("Called");
    
    ret = Stop(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        LogE("Stop() - super::Stop failed with error: 0x%08x.", ret);
        return ret;
    }

    LogI("Finished");

    return ret;
}

void UsbBlockerDriver::free(void)
{
    LogI("Called");

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
    LogI("Called");

    ret = Create(this, "UserClientProperties", &client);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to create UserClientProperties with error: 0x%08x.", ret);
        return ret;
    }

    *userClient = OSDynamicCast(UsbBlockerUserClient, client);
    if (*userClient == NULL)
    {
        LogE("Failed to cast new client.");
        client->release();
        return ret;
    }
    
    UsbBlockerUserClient* dextClient = OSRequiredCast(UsbBlockerUserClient, client);
    dextClient->SetParent(this);

    LogI("Finished");
    return ret;
}

void UsbBlockerDriver::ObtainConfigurationValue()
{
    LogI("Started");
    bool sucessful = true;
    ivars->configValue = 1;
    OSContainer* config = nullptr;
    OSContainer* vendor = nullptr;
    OSContainer* product = nullptr;
    
    OSNumber* configNumber = nullptr;
    OSNumber* vendorID = nullptr;
    OSNumber* productID = nullptr;
    
    IOUSBConfigurationDescriptor* configDesc = nullptr;
    
    int index = 0;
    
    kern_return_t ret = ivars->usbDevice->SearchProperty(kUSBHostDevicePropertyNumConfigs, "", 0, &config);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to obtain number of configurations. Result - %d; 0x%08x", ret, ret);
        goto Release;
    }
    
    configNumber = OSRequiredCast(OSNumber, config);
    LogI("Obtained kUSBHostDevicePropertyNumConfigs - %d", configNumber->unsigned8BitValue());

    configDesc = const_cast<IOUSBConfigurationDescriptor*>(ivars->usbDevice->CopyConfigurationDescriptor(index));
    if (configDesc == nullptr)
    {
        LogE("Failed to get configuration descriptor. Use default");
        goto Release;
    }
    
    if (configDesc->bConfigurationValue != 0)
    {
        ivars->configValue = configDesc->bConfigurationValue;
        LogI("Obtained bConfigurationValue - %d", configDesc->bConfigurationValue);
    }
    else
    {
        LogI("Obtained bConfigurationValue is equal to 0. Use default.");
    }
    
    ret = ivars->usbDevice->SearchProperty(kUSBHostMatchingPropertyVendorID, "", 0, &vendor);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to obtain vendorID. Result - %d; 0x%08x", ret, ret);
        goto Release;
    }
    
    vendorID = OSRequiredCast(OSNumber, vendor);
    LogI("Obtained VendorID - %d", vendorID->unsigned16BitValue());
    if (vendorID->unsigned16BitValue() == 0)
    {
        LogE("Obtained wrong vendorID");
        goto Release;
    }
    
    ivars->vendorID = vendorID->unsigned16BitValue();
    
    ret = ivars->usbDevice->SearchProperty(kUSBHostMatchingPropertyProductID, "", 0, &product);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to obtain ProductID. Result - %d; 0x%08x", ret, ret);
        goto Release;
    }
    
    productID = OSRequiredCast(OSNumber, product);
    LogI("Obtained ProductID - %d", productID->unsigned16BitValue());
    if (productID->unsigned16BitValue() == 0)
    {
        LogE("Obtained wrong ProductID");
        goto Release;
    }
    
    ivars->productID = productID->unsigned16BitValue();
    
Release:
    if (config != nullptr)
    {
        OSSafeReleaseNULL(config);
    }

    if (vendor != nullptr)
    {
        OSSafeReleaseNULL(vendor);
    }
    
    if (product != nullptr)
    {
        OSSafeReleaseNULL(product);
    }
    
    if (sucessful)
    {
        LogI("ObtainConfigurationValue finished");
    }
}

bool UsbBlockerDriver::ConfigureUSBDevice(bool enable)
{
    LogI("ConfigureUSBDevice called");
    if (ivars->usbDevice == nullptr)
    {
        LogE("Failed to change IOUSBHostDevice state the ivars->usbDevice is nullptr.");
        return false;
    }

    kern_return_t ret = ivars->usbDevice->Open(this, 0, NULL);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to open connection to usbDevice object. result - %d; 0x%08x", ret, ret);
        return false;
    }
    
    ret = ivars->usbDevice->SetConfiguration((enable ? ivars->configValue : 0), enable);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to SetConfiguration for usbDevice object. result - %d; 0x%08x", ret, ret);
        return false;
    }
    
    ret = ivars->usbDevice->Close(this, 0);
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to close connection to usbDevice object. result - %d; 0x%08x", ret, ret);
        return false;
    }
    
    LogI("ConfigureUSBDevice finished");
    return true;
}

uint16_t UsbBlockerDriver::GetVendorID() {
    return ivars->vendorID;
}

uint16_t UsbBlockerDriver::GetProductID() {
    return ivars->productID;
}
