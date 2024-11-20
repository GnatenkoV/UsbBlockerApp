//
//  DextCommunicationManager.swift
//  UsbBlocker
//

import Foundation

class DextCommunicationManager {
    private let _dextMainClassName = "Driver"
    private var _connection: io_connect_t = io_connect_t()
    private var _connectionEstablished = false
    private let _configureUsbDeviceSelector = 0
    
    func setDeviceState(_ enabled: Bool) -> Bool {
        var ret = kIOReturnSuccess
        var result: UInt64 = 0
        var outputCount: UInt32 = 1
        let input = [UInt64(enabled ? 1 : 0)]
        if !_connectionEstablished {
            if !establishConnectionWithDext() {
                return false
            }
        }
        
        ret = IOConnectCallScalarMethod(_connection, UInt32(_configureUsbDeviceSelector), input, 1, &result, &outputCount)
        if ret != kIOReturnSuccess {
            NSLog("Failed to set device state, ret: 0x%008x", ret)
            return false
        }
        
        return true
    }
    
    private func establishConnectionWithDext() -> Bool {
        var ret = kIOReturnError
        var iterator: io_iterator_t = io_iterator_t()
        var service: io_service_t = IO_OBJECT_NULL
        
        ret = IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceNameMatching(_dextMainClassName), &iterator)
        if ret != kIOReturnSuccess {
            NSLog("Unable to find service for identifier: %s with error: 0x%08x.", _dextMainClassName, ret)
            return false;
        }
        
        service = IOIteratorNext(iterator)
        while (service != IO_OBJECT_NULL) {
            // Open a connection to this user client as a server to that client, and store the instance in "service"
            ret = IOServiceOpen(service, mach_task_self_, UInt32(kIOHIDServerConnectType), &_connection)
            if (ret == kIOReturnSuccess) {
                _connectionEstablished = true
                break;
            }
           
            NSLog("Failed opening service with error: 0x%08x.", ret);
            IOObjectRelease(service);
            service = IOIteratorNext(iterator)
        }
        
        IOObjectRelease(iterator)
        
        if ret != kIOReturnSuccess {
            NSLog("Failed to match to driver.")
        }
        
        return ret == kIOReturnSuccess
    }
}
