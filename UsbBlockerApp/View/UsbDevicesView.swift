import SwiftUI

struct UsbDevicesView: View {
    @Binding public var devices: [UsbDevice]
    @ObservedObject public var devicesManager: UsbDevicesManager
    
    private func updateDevice(_ device: UsbDevice) {
        devicesManager.updateDeviceState(device)
    }
    
    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            List() {
                ForEach(self.$devices) { device in
                    HStack {
                        
                        Toggle("", isOn: device.enabled)
                            .onChange(of: device.enabled.wrappedValue) {
                                updateDevice(device.wrappedValue)
                            }
                        Text(device.deviceName.wrappedValue)
                        
                        Text(device.id)
                        .buttonStyle(.borderless)
                        .foregroundColor(.accentColor)
                        .frame(maxWidth: .infinity, alignment: .trailing)
                        .padding(.trailing, 19)
                    }
                }
            }
        }
    }
}
