//
//  DextInstallationManager.swift
//  UsbBlocker
//

import Foundation
import SystemExtensions

class DextInstallationManager: NSObject, OSSystemExtensionRequestDelegate {
    //MARK: - Private Variables
    private lazy var _extensionBundle: Bundle = {
        let extensionsDirectoryURL = URL(fileURLWithPath: "Contents/Library/SystemExtensions", relativeTo: Bundle.main.bundleURL)
        let extensionURLs: [URL]
        do {
            extensionURLs = try FileManager.default.contentsOfDirectory(at: extensionsDirectoryURL,
                                                                        includingPropertiesForKeys: nil,
                                                                        options: .skipsHiddenFiles)
        } catch let error {
            fatalError("Failed to get the contents of \(extensionsDirectoryURL.absoluteString):\n\(error.localizedDescription)")
        }

        guard let extensionURL = extensionURLs.first else {
            fatalError("Failed to find any system extensions")
        }

        guard let extensionBundle = Bundle(url: extensionURL) else {
            fatalError("Failed to create a bundle with URL:\n\(extensionURL.absoluteString)")
        }

        print("extensionBundle: \(String(describing: extensionBundle.bundleIdentifier))")
        
        return extensionBundle
    }()
    
    func activate() {
        print("DextInstallationManager activate called ")
        print("Begin installing the extension: \(String(describing: _extensionBundle.bundleIdentifier)) 🔄\n")
        let activationRequest = OSSystemExtensionRequest.activationRequest(forExtensionWithIdentifier: _extensionBundle.bundleIdentifier ?? "", queue: .main)
        activationRequest.delegate = self
        OSSystemExtensionManager.shared.submitRequest(activationRequest)
    }
    
    func deactivate() {
        let deactivationRequest = OSSystemExtensionRequest.deactivationRequest(forExtensionWithIdentifier: _extensionBundle.bundleIdentifier ?? "", queue: .main)
        deactivationRequest.delegate = self
        OSSystemExtensionManager.shared.submitRequest(deactivationRequest)
    }
    
    func request(_ request: OSSystemExtensionRequest, actionForReplacingExtension existing: OSSystemExtensionProperties, withExtension ext: OSSystemExtensionProperties) -> OSSystemExtensionRequest.ReplacementAction {
        if let existingVersion = Int(existing.bundleVersion), let replacementVersion = Int(ext.bundleVersion),
            existingVersion >= replacementVersion {
            print("Got the upgrade request \(existing) -> \(ext); answering cancel.")
            return .cancel
        }
        
        print("Got the upgrade request \(existing) -> \(ext); answering replace.")
        return .replace
    }
    
    func requestNeedsUserApproval(_ request: OSSystemExtensionRequest) {
        print("Request to activate. Awaiting Approval ⏱\n")
    }
    
    func request(_ request: OSSystemExtensionRequest, didFinishWithResult result: OSSystemExtensionRequest.Result) {
        print("Success ✅\n")
    }
    
    func request(_ request: OSSystemExtensionRequest, didFailWithError error: Error) {
        print("Failed ❌\n\(error.localizedDescription)")
    }
}
