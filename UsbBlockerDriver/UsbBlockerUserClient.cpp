#include "pch.h"
#include <dlfcn.h>
#include <DriverKit/IOLib.h>
#include <DriverKit/OSDictionary.h>
#include <DriverKit/IOUserClient.h>

#include "UsbBlockerUserClient.h"

union UsbDeviceInfo {
    uint64_t value;
    uint16_t elements[4];
};

struct UsbBlockerUserClient_IVars {
    UsbBlockerDriver* parentDext;
    OSAction* callbackAction;
};

typedef enum {
    ExternalMethodType_ConfigureUSBDevice = 0,
    NumberOfExternalMethods // Has to be last
} ExternalMethodType;

const IOUserClientMethodDispatch externalMethodChecks[NumberOfExternalMethods] =
{
    [ExternalMethodType_ConfigureUSBDevice] =
    {
        .function = (IOUserClientMethodFunction) &UsbBlockerUserClient::StaticConfigureUSBDevice,
        .checkCompletionExists = false,
        .checkScalarInputCount = 1,
        .checkStructureInputSize = 0,
        .checkScalarOutputCount = 1,
        .checkStructureOutputSize = 0,
    },
};

// MARK: Dext Lifecycle Management
bool UsbBlockerUserClient::init(void) {
    bool result = false;

    LogI("Called");
    result = super::init();
    if (result != true)
    {
        LogE("super::init failed.");
        goto Exit;
    }

    ivars = IONewZero(UsbBlockerUserClient_IVars, 1);
    if (ivars == nullptr)
    {
        LogE("Failed to allocate memory for ivars.");
        goto Exit;
    }

    LogI("Finished.");
    return true;

Exit:
    return false;
}

kern_return_t IMPL(UsbBlockerUserClient, Start) {
    kern_return_t ret = kIOReturnSuccess;
    
    LogI("Called");
    ret = Start(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        LogE("super::Start failed with error: 0x%08x.", ret);
        goto Exit;
    }

    ret = RegisterService();
    if (ret != kIOReturnSuccess)
    {
        LogE("Failed to register service with error: 0x%08x.", ret);
        goto Exit;
    }

    LogI("Finished.");
    ret = kIOReturnSuccess;

Exit:
    return ret;
}

kern_return_t IMPL(UsbBlockerUserClient, Stop) {
    kern_return_t ret = kIOReturnSuccess;
    
    LogI("Called");
    ret = Stop(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        LogE("super::Stop failed with error: 0x%08x.", ret);
        return ret;
    }
    
    LogI("Finished.");
    
    return ret;
}

void UsbBlockerUserClient::free(void) {
    LogI("Called");
    if (ivars)
    {
        OSSafeReleaseNULL(ivars->callbackAction);
    }
    
    IOSafeDeleteNULL(ivars, UsbBlockerUserClient_IVars, 1);
    super::free();

    LogI("Finished");
}

void UsbBlockerUserClient::SetParent(IOService* parent)
{
    UsbBlockerDriver *parentObject = OSRequiredCast(UsbBlockerDriver, parent);
    ivars->parentDext = parentObject;
}

kern_return_t UsbBlockerUserClient::ExternalMethod(uint64_t selector, IOUserClientMethodArguments* arguments, const IOUserClientMethodDispatch* dispatch, OSObject* target, void* reference)
{
    LogI("Called");
    if (selector < NumberOfExternalMethods)
    {
        dispatch = &externalMethodChecks[selector];
        if (!target)
        {
            target = this;
        }
    }
    
    return super::ExternalMethod(selector, arguments, dispatch, target, reference);
}

kern_return_t UsbBlockerUserClient::StaticConfigureUSBDevice(OSObject* target, void* reference, IOUserClientMethodArguments* arguments)
{
    LogI("Called");
    if (target == nullptr)
    {
        LogE("Target is nullptr.");
        return kIOReturnError;
    }

    return ((UsbBlockerUserClient*)target)->ConfigureUSBDevice(reference, arguments);
}

kern_return_t UsbBlockerUserClient::ConfigureUSBDevice(void* reference, IOUserClientMethodArguments* arguments)
{
    LogI("Input value: %llu", arguments->scalarInput[0]);
    
    if (ivars->parentDext == nullptr)
    {
        LogE("ConfigureUSBDevice ivars->parentDext is invalid");
        return kIOReturnError;
    }
    
    UsbDeviceInfo info = static_cast<const UsbDeviceInfo>(arguments->scalarInput[0]);
    
    LogI("Parsed input - %d:%d:%d:%d", info.elements[0], info.elements[1], info.elements[2], info.elements[3]);
    
    if (info.elements[2] == ivars->parentDext->GetVendorID() && info.elements[3] == ivars->parentDext->GetProductID())
    {
        bool isEnabled = info.elements[0] != 0;
        bool result = ivars->parentDext->ConfigureUSBDevice(isEnabled);
        arguments->scalarOutput[0] = result;
        LogI("USB device configuration %{public}s.", result ? "succeeded" : "failed");
    }
    else
    {
        arguments->scalarOutput[0] = false;
    }
    
    return kIOReturnSuccess;
}
