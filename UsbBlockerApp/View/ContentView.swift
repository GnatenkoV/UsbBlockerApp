//
//  ContentView.swift
//  UsbBlockerApp
//
//  Created by user on 19.11.2024.
//

import SwiftUI

struct ContentView: View {
    @ObservedObject public var devicesManager: UsbDevicesManager
    @ObservedObject public var installationManager: InstallationManager
    
    var body: some View {
        NavigationSplitView {
            SidebarView(installationManager: _installationManager, devicesManager: _devicesManager)
        } detail: {
            if devicesManager.devices.count > 0 {
                switch self.devicesManager.filterSelection {
                case .all:
                    UsbDevicesView(devices: $devicesManager.devices, devicesManager: devicesManager)
                case .enabled:
                    UsbDevicesView(devices: $devicesManager.devices.filter({ $0.enabled }), devicesManager: devicesManager)
                case .disabled:
                    UsbDevicesView(devices:  $devicesManager.devices.filter({ !$0.enabled }), devicesManager: devicesManager)
                }
            } else {
                Text("Loading devices")
            }
        }
    }
}

#Preview {
    ContentView(devicesManager: UsbDevicesManager(), installationManager: InstallationManager())
}
