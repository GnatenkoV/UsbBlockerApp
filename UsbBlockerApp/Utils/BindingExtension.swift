//
//  BindingExtension.swift
//  UsbBlocker
//
//  Created by user on 19.11.2024.
//

import SwiftUI

// took from https://gist.github.com/jemmons/3b0f9442e75f9126f9f4082ae4b49fa7
extension Binding where Value: MutableCollection, Value: RangeReplaceableCollection, Value.Element: Identifiable {
    
    func filter(_ isIncluded: @escaping (Value.Element)->Bool) -> Binding<[Value.Element]> {
        return Binding<[Value.Element]>(
            get: {
                // The binding returns a filtered subset of the original wrapped collection.
                self.wrappedValue.filter(isIncluded)
            },
            set: { newValue in
                // Assignments to the binding's wrapped value are compared with IDs in the original binding's collection.
                // If they match, they are replaced.
                // If no match is found, they are appended.
                newValue.forEach { newItem in
                    guard let i = self.wrappedValue.firstIndex(where: { $0.id == newItem.id }) else {
                        self.wrappedValue.append(newItem)
                        return
                    }
                    self.wrappedValue[i] = newItem
                }
            }
        )
    }
}
