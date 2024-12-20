//
//  FilterSelection.swift
//  UsbBlockerApp
//
//  Created by user on 19.11.2024.
//


public enum FilterSelection: Identifiable, CaseIterable, Hashable {
    case all
    case enabled
    case disabled
    
    public var id: String {
        switch (self) {
        case .all:
            "all"
        case .enabled:
            "enabled"
        case .disabled:
            "disabled"
        }
    }
    
    public var displayName: String {
        switch (self) {
        case .all:
            "All"
        case .enabled:
            "Enabled"
        case .disabled:
            "Disabled"
        }
    }
    
    public var iconName: String {
        switch self {
        case .all:
            "star"
        case .enabled:
            "togglepower"
        case .disabled:
            "poweroff"
        }
    }
    
    public static var allCases: [FilterSelection] {
        [.all, .enabled, .disabled]
    }
    
    public static func == (lhs: FilterSelection, rhs: FilterSelection) -> Bool {
        lhs.id == rhs.id
    }
}
