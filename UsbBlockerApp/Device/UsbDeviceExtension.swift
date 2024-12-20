import Foundation

public extension UsbDevice {    
    static func emptyRule() -> UsbDevice {
        return UsbDevice(deviceName: "", manufacturerName: "", enabled: true, vendorId: 0, productId: 0)
    }
    
    static func update(deviceOut: inout UsbDevice, deviceIn: UsbDevice) -> Bool {
        let updated = deviceOut.deviceName != deviceIn.deviceName || deviceOut.enabled != deviceIn.enabled || deviceOut.vendorId != deviceIn.vendorId || deviceOut.productId != deviceIn.productId || deviceIn.manufgarturerName != deviceOut.manufgarturerName
        
        deviceOut.deviceName = deviceIn.deviceName
        deviceOut.manufgarturerName = deviceIn.manufgarturerName
        deviceOut.enabled = deviceIn.enabled
        deviceOut.vendorId = deviceIn.vendorId
        deviceOut.productId = deviceIn.productId
        
        return updated
    }
}
