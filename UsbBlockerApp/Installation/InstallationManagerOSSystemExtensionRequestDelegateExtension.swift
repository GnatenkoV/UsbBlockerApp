import Foundation
import OSLog
import SystemExtensions

extension InstallationManager: OSSystemExtensionRequestDelegate {
    @objc func request(_ request: OSSystemExtensionRequest, didFailWithError error: Error) {
        os_log("System extension request failed %@", error.localizedDescription)
        
        if (self.uninstallingInProgress) {
            exit(0)
        }
        
        self.status = getExtensionStatus()
    }
      
      
    @objc func requestNeedsUserApproval(_ request: OSSystemExtensionRequest) {
        os_log("System extension requires user approval")
        
        self.status = getExtensionStatus()
    }
      
      
    @objc func request(_ request: OSSystemExtensionRequest,
                 actionForReplacingExtension existing: OSSystemExtensionProperties,
                 withExtension ext: OSSystemExtensionProperties) -> OSSystemExtensionRequest.ReplacementAction {
        os_log("Replacing extension: %@ %@", existing, ext)
        
        self.status = getExtensionStatus()
        
        return .replace
    }
      
      
    @objc func request(_ request: OSSystemExtensionRequest, didFinishWithResult result: OSSystemExtensionRequest.Result) {
        os_log("System extension activating request result: %d", result.rawValue)
        
        if (self.uninstallingInProgress) {
            exit(0)
        }
        
        self.status = getExtensionStatus()
    }
}
