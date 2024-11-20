//
//  ViewController.swift
//  UsbBlocker
//

import Cocoa

class ViewController: NSViewController {
    @IBOutlet weak var installDextButton: NSButton!
    @IBOutlet weak var enableDeviceButton: NSButton!
    @IBOutlet weak var disableDeviceButton: NSButton!
    private let _dextInstallationManager = DextInstallationManager()
    private let _dextCommunicationManager = DextCommunicationManager()

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    @IBAction func installButtonClicked(_ sender: NSButton) {
        _dextInstallationManager.activate()
    }
    
    @IBAction func enableDeviceButtonClicked(_ sender: NSButton) {
        let result = _dextCommunicationManager.setDeviceState(true)
        if !result {
            NSLog("USBBlocker Failed to enable device")
        }
    }
    
    @IBAction func disableDeviceButtonClicked(_ sender: NSButton) {
        let result = _dextCommunicationManager.setDeviceState(false)
        if !result {
            NSLog("USBBlocker Failed to disable device")
        }
    }
}
