import Foundation
import SystemExtensions
import OSLog

class InstallationManager: NSObject, ObservableObject
{
    private let extensionBundleId: String = "com.UsbBlocker.Driver"
    private var currentRequest: OSSystemExtensionRequest?
    
    @Published public var status = ExtensionStatus.uninstalled
    @Published public var uninstallingInProgress = false
    
    override init() {
        super.init()

        self.status = getExtensionStatus()
        
        os_log(OSLogType.debug, "args: \(CommandLine.arguments)")
        
        if (CommandLine.arguments.contains("-uninstall")) {
            uninstallingInProgress = true
            
            if (status == ExtensionStatus.installed || status == ExtensionStatus.wait_approve) {
                uninstall()
            } else {
                exit(0)
            }
        }
    }
    
    public func getExtensionStatus() -> ExtensionStatus {
        return ExtensionUtils.getExtensionStatus(extensionBundleId: extensionBundleId)
    }
    
    public func install() {
        status = ExtensionStatus.uninstalled
        
        let request = OSSystemExtensionRequest.activationRequest(forExtensionWithIdentifier: extensionBundleId, queue: DispatchQueue.main)
        request.delegate = self
        
        currentRequest = request
        
        OSSystemExtensionManager.shared.submitRequest(request)
    }
    
    public func uninstall() {
        status = ExtensionStatus.uninstalled
        
        let request = OSSystemExtensionRequest.deactivationRequest(forExtensionWithIdentifier: extensionBundleId, queue: DispatchQueue.main)
        request.delegate = self
        
        OSSystemExtensionManager.shared.submitRequest(request)
    }
}
