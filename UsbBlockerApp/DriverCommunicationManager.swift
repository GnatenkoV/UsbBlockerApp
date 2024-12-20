import Foundation

struct DeviceInfo {
    var state: UInt32
    var vendorId: UInt16
    var productId: UInt16
    
    var union: UInt64 {
        get { return (UInt64(vendorId) << 32) | (UInt64(productId) << 48) | UInt64(state) }
        set {
            self.state = UInt32(newValue & 0xFFFFFFFF)
            self.vendorId = UInt16((newValue & 0xFFFFFFFF) >> 32)
            self.productId = UInt16((newValue & 0xFFFFFFFF) >> 48)
        }
    }
}

class DriverCommunicationManager {
    private let _dextMainClassName = "UsbBlockerDriver"
    private var _connections = [io_connect_t]()
    private var _connectionEstablished = false
    private let _configureUsbDeviceSelector = 0
    
    deinit {
        releaseConnectionWithDext()
    }
    
    func setDeviceState(_ device: UsbDevice) -> Bool {
        var ret = kIOReturnSuccess
        var result: UInt64 = 0
        var outputCount: UInt32 = 1
        let input = DeviceInfo(state: (device.enabled ? UInt32(1) : UInt32(0)), vendorId: UInt16(device.vendorId), productId: UInt16(device.productId))
        let union = [input.union]
        if !_connectionEstablished {
            if !establishConnectionWithDext() {
                return false
            }
        }
        
        for connection in _connections {
            ret = IOConnectCallScalarMethod(connection, UInt32(_configureUsbDeviceSelector), union, 1, &result, &outputCount)
            if ret != kIOReturnSuccess {
                NSLog("Failed to set device state, ret: 0x%008x", ret)
            }
        }

        return true
    }
    
    private func releaseConnectionWithDext() {
        for connection in _connections {
            IOServiceClose(connection)
        }
        _connectionEstablished = false
        _connections.removeAll()
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
        
        var connection: io_connect_t = io_connect_t()
        while (service != IO_OBJECT_NULL) {
            // Open a connection to this user client as a server to that client, and store the instance in "service"
            ret = IOServiceOpen(service, mach_task_self_, UInt32(kIOHIDServerConnectType), &connection)
            if (ret != kIOReturnSuccess) {
                NSLog("Failed opening service with error: 0x%08x.", ret);
                break;
            }
           
            _connections.append(connection)
            
            IOObjectRelease(service);
            service = IOIteratorNext(iterator)
        }
        
        _connectionEstablished = true
        
        IOObjectRelease(iterator)
        
        if ret != kIOReturnSuccess {
            NSLog("Failed to match to driver.")
        }
        
        return ret == kIOReturnSuccess
    }
}
