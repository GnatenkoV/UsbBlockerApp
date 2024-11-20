//
//  DriverUserClient.cpp
//  Driver
//

#include <os/log.h>

#include <DriverKit/IOLib.h>
#include <DriverKit/OSDictionary.h>
#include <DriverKit/IOUserClient.h>

#include "DriverUserClient.h"

#define Log(fmt, ...) os_log(OS_LOG_DEFAULT, "AprioritDriver UserClient - " fmt "\n", ##__VA_ARGS__)

struct DriverUserClient_IVars
{
    Driver* parentDext;
    OSAction* callbackAction;
};

typedef enum
{
    ExternalMethodType_ConfigureUSBDevice = 0,
    NumberOfExternalMethods // Has to be last
} ExternalMethodType;

const IOUserClientMethodDispatch externalMethodChecks[NumberOfExternalMethods] =
{
    [ExternalMethodType_ConfigureUSBDevice] =
    {
        .function = (IOUserClientMethodFunction) &DriverUserClient::StaticConfigureUSBDevice,
        .checkCompletionExists = false,
        .checkScalarInputCount = 1,
        .checkStructureInputSize = 0,
        .checkScalarOutputCount = 1,
        .checkStructureOutputSize = 0,
    },
};

// MARK: Dext Lifecycle Management
bool DriverUserClient::init(void)
{
    bool result = false;

    Log("init()");

    result = super::init();
    if (result != true)
    {
        Log("init() - super::init failed.");
        goto Exit;
    }

    ivars = IONewZero(DriverUserClient_IVars, 1);
    if (ivars == nullptr)
    {
        Log("init() - Failed to allocate memory for ivars.");
        goto Exit;
    }

    Log("init() - Finished.");
    return true;

Exit:
    return false;
}

kern_return_t
IMPL(DriverUserClient, Start)
{
    kern_return_t ret = kIOReturnSuccess;

    ret = Start(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        Log("Start() - super::Start failed with error: 0x%08x.", ret);
        goto Exit;
    }

    ret = RegisterService();
    if (ret != kIOReturnSuccess)
    {
        Log("Start() - Failed to register service with error: 0x%08x.", ret);
        goto Exit;
    }

    Log("Start() - Finished.");
    ret = kIOReturnSuccess;

Exit:
    return ret;
}

kern_return_t
IMPL(DriverUserClient, Stop)
{
    kern_return_t ret = kIOReturnSuccess;
    Log("Stop()");
    
    ret = Stop(provider, SUPERDISPATCH);
    if (ret != kIOReturnSuccess)
    {
        Log("Stop() - super::Stop failed with error: 0x%08x.", ret);
        return ret;
    }
    
    Log("Stop() - Finished.");
    
    return ret;
}

void DriverUserClient::free(void)
{
    Log("free()");

    if (ivars)
    {
        OSSafeReleaseNULL(ivars->callbackAction);
    }
    
    IOSafeDeleteNULL(ivars, DriverUserClient_IVars, 1);
    super::free();

    Log("free finished");
}

void DriverUserClient::SetParent(IOService* parent)
{
    Driver *parentObject = OSRequiredCast(Driver, parent);
    ivars->parentDext = parentObject;
}

kern_return_t DriverUserClient::ExternalMethod(uint64_t selector, IOUserClientMethodArguments* arguments, const IOUserClientMethodDispatch* dispatch, OSObject* target, void* reference)
{
    Log("ExternalMethod called");
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

kern_return_t DriverUserClient::StaticConfigureUSBDevice(OSObject* target, void* reference, IOUserClientMethodArguments* arguments)
{
    Log("StaticConfigureUSBDevice called");
    if (target == nullptr)
    {
        Log("StaticConfigureUSBDevice target is nullptr.");
        return kIOReturnError;
    }

    return ((DriverUserClient*)target)->ConfigureUSBDevice(reference, arguments);
}

kern_return_t DriverUserClient::ConfigureUSBDevice(void* reference, IOUserClientMethodArguments* arguments)
{
    Log("ConfigureUSBDevice input value: %llu", arguments->scalarInput[0]);
    
    if (ivars->parentDext == nullptr)
    {
        Log("ConfigureUSBDevice ivars->parentDext is invalid");
        return kIOReturnError;
    }
    
    bool isEnabled = arguments->scalarInput[0] != 0;
    bool result = ivars->parentDext->ConfigureUSBDevice(isEnabled);
    arguments->scalarOutput[0] = result;
    Log("USB device configuration %{public}s.", result ? "succeeded" : "failed");
    
    return kIOReturnSuccess;
}
