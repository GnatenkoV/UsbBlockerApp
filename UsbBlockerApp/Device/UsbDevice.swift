import Foundation

public struct UsbDevice : Identifiable, Hashable, Codable {
    public var id: String {
        return String(format: "0x%.4x:0x%.4x", vendorId, productId)
    }
    public var deviceName: String
    public var manufgarturerName: String
    public var enabled: Bool
    public var vendorId: Int
    public var productId: Int
    
    public init(deviceName: String, manufacturerName: String, enabled: Bool, vendorId: Int, productId: Int) {
        self.deviceName = deviceName
        self.manufgarturerName = manufacturerName
        self.enabled = enabled
        self.vendorId = vendorId
        self.productId = productId
    }
    
    public init(device: UsbDevice) {
        self.deviceName = device.deviceName
        self.manufgarturerName = device.manufgarturerName
        self.enabled = device.enabled
        self.vendorId = device.vendorId
        self.productId = device.productId
    }
}
