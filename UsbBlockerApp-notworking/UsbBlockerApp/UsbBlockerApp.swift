//
//  UsbBlockerAppApp.swift
//  UsbBlockerApp
//
//  Created by user on 20.11.2024.
//

import SwiftUI

@main
struct UsbBlockerAppApp: App {
    var installationManager = DextInstallationManager()
    
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .commands {
            CommandMenu("Extension")
            {
                Button("Install", action: {
                    self.installationManager.activate()
                })
                .keyboardShortcut(KeyEquivalent("i"), modifiers: [.command])
                
                Button("Uninstall", action: {
                    self.installationManager.deactivate()
                })
                .keyboardShortcut(KeyEquivalent("u"), modifiers: [.command])
            }
        }
    }
}
