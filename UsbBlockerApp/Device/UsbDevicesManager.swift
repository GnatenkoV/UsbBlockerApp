import Foundation

@MainActor
public class UsbDevicesManager: NSObject, ObservableObject {
    @Published public var devices: [UsbDevice]
    @Published public var filterEnabled: Bool = false
    @Published public var filterSelection: FilterSelection = FilterSelection.all
    @Published public var devicesChanged = false
    
    private var driverCommunication: DriverCommunicationManager
    
    override init() {
        self.devices = [UsbDevice]()
        self.driverCommunication = DriverCommunicationManager()
        super.init()
        
        self.updateDevices()
    }
    
    public func updateDevices() {
        self.devices.removeAll()
        self.devices.append(contentsOf: getDevices())
    }
    
    public func updateDeviceState(_ device: UsbDevice) {
        let result = self.driverCommunication.setDeviceState(device)
        print("result \(result)")
    }
    
    private func getDevices() -> [UsbDevice] {
        var result = [UsbDevice]()
        let notificationPort: IONotificationPortRef? = IONotificationPortCreate(kIOMainPortDefault)
        let matchingDict = IOServiceMatching(kIOUSBDeviceClassName)
        
        var devicesIterator: io_iterator_t = IO_OBJECT_NULL
        let callbackClosure: IOServiceMatchingCallback = { _, __ in
        }

        let notifierResult = IOServiceAddMatchingNotification(notificationPort, kIOMatchedNotification, matchingDict, callbackClosure, Unmanaged.passUnretained(self).toOpaque(), &devicesIterator)
        
        if notifierResult == kIOReturnSuccess {
            result = UsbDeviceHelper.parseIterator(devicesIterator)
        }
        
        // release port
        IONotificationPortDestroy(notificationPort)
        // release iterator
        IOObjectRelease(devicesIterator)
        
        return result
    }
}
