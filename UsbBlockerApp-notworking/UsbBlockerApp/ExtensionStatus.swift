//
//  ContentView.swift
//  UsbBlockerApp
//
//  Created by user on 19.11.2024.
//

public enum ExtensionStatus: Identifiable, CaseIterable, Hashable {
    case installed
    case uninstalled
    case wait_approve
    
    public var id: String {
        switch (self) {
        case .installed:
            "installed"
        case .uninstalled:
            "uninstalled"
        case .wait_approve:
            "wait_approve"
        }
    }
    
    public var displayName: String {
        switch (self) {
        case .installed:
            "Installed"
        case .uninstalled:
            "Uninstalled"
        case .wait_approve:
            "Wait Approve"
        }
    }
    
    public var iconName: String {
        switch self {
        case .installed:
            "checkmark.circle"
        case .uninstalled:
            "xmark.circle"
        case .wait_approve:
            "checkmark.circle.trianglebadge.exclamationmark"
        }
    }
    
    public var tooltip: String {
        switch self {
        case .installed:
            "Installed"
        case .uninstalled:
            "Not installed"
        case .wait_approve:
            "Waiting for approve"
        }
    }
    
    public static func == (lhs: ExtensionStatus, rhs: ExtensionStatus) -> Bool {
        lhs.id == rhs.id
    }
}
