//
//  UsbBlockerAppApp.swift
//  UsbBlockerApp
//
//  Created by user on 19.11.2024.
//

import SwiftUI

@main
struct UsbBlockerAppApp: App {
    @ObservedObject var devicesManager = UsbDevicesManager()
    @ObservedObject var installationManager = InstallationManager()
    
    var body: some Scene {
        WindowGroup {
            ContentView(devicesManager: devicesManager, installationManager: installationManager)
        }
        .commands {            
            CommandMenu("Extension")
            {
                Button("Install", action: {
                    self.installationManager.install()
                })
                .keyboardShortcut(KeyEquivalent("i"), modifiers: [.command])
                
                Button("Uninstall", action: {
                    self.installationManager.uninstall()
                })
                .keyboardShortcut(KeyEquivalent("u"), modifiers: [.command])
            }
        }
    }
}
