import Foundation
import IOKit.usb

private class UsbBlankClass {
    // just blank class to pass the pointer into IOServiceAddMatchingNotification
}

public class UsbDeviceHelper {
    public static func parseIterator(_ iterator: io_iterator_t) -> [UsbDevice] {
        var devices = [UsbDevice]()

        var device = IOIteratorNext(iterator)
        while device != 0 {
            var name = "Unknown name"
            var manufacturer = "Unknown manufacturer"
            var vendorID = 0
            var productID = 0
            var configValue = 0
            
            if let nameCFProperty = IORegistryEntryCreateCFProperty(device, kUSBProductString as CFString, kCFAllocatorDefault, 0) {
                name = (nameCFProperty.takeRetainedValue() as? String) ?? "Unknown name"
            }
            
            if let manufacturerCF = IORegistryEntryCreateCFProperty(device, kUSBVendorString as CFString, kCFAllocatorDefault, 0) {
                manufacturer = (manufacturerCF.takeRetainedValue() as? String) ?? "Unknown manufacturer"
            }
            
            if let vendorIDCFProperty = IORegistryEntryCreateCFProperty(device, kUSBVendorID as CFString, kCFAllocatorDefault, 0) {
                vendorID = (vendorIDCFProperty.takeRetainedValue() as? NSNumber)?.intValue ?? 0
            }

            if let productIDCFProperty = IORegistryEntryCreateCFProperty(device, kUSBProductID as CFString, kCFAllocatorDefault, 0) {
                productID = (productIDCFProperty.takeRetainedValue() as? NSNumber)?.intValue ?? 0
            }
            
            let conf = "kUSBCurrentConfiguration"
            
            if let configValueCFProperty = IORegistryEntryCreateCFProperty(device, conf as CFString, kCFAllocatorDefault, 0) {
                configValue = (configValueCFProperty.takeRetainedValue() as? NSNumber)?.intValue ?? 0
            }
            
            devices.append(UsbDevice(deviceName: name, manufacturerName: manufacturer, enabled: configValue != 0, vendorId: vendorID, productId: productID))

            // Release the device object
            IOObjectRelease(device)
            
            // Move to the next object
            device = IOIteratorNext(iterator)
        }

        return devices
    }
}
