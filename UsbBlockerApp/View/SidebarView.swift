import SwiftUI

struct SidebarView: View {
    @ObservedObject var installationManager: InstallationManager
    @ObservedObject var devicesManager: UsbDevicesManager
    
    init(installationManager: ObservedObject<InstallationManager>, devicesManager: ObservedObject<UsbDevicesManager>) {
        self._installationManager = installationManager
        self._devicesManager = devicesManager
    }
    
    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            List(selection: self.$devicesManager.filterSelection) {
                Section("Filters") {
                    ForEach(FilterSelection.allCases) { selection in
                        Label(selection.displayName, systemImage: selection.iconName)
                            .tag(selection)
                    }
                }
            }
            .frame(height: 120, alignment: .top)
            .scrollDisabled(true)
            
            Spacer()
            
            VStack (alignment: .leading, spacing: 14) {
                Rectangle()
                    .fill(.gray)
                    .frame(maxWidth: .infinity, minHeight: 1, idealHeight: 1, maxHeight: 1)
                    .opacity(0.3)
                .buttonStyle(.borderless)
                .foregroundColor(.accentColor)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.leading, 19)
                
                Label("Installed", systemImage: installationManager.status.iconName)
                    .help(Text(installationManager.status.tooltip))
                    .padding(.leading, 19)
                
                Button(action: {
                    self.devicesManager.updateDevices()
                }, label: {
                    Text("Update devices")
                })
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.leading, 19)
                .padding(.bottom, 15)
            }
        }
    }
}

#if DEBUG
struct SidebarView_Preview : PreviewProvider {
    @ObservedObject static var devicesManager = UsbDevicesManager()
    @ObservedObject static var installationManager = InstallationManager()
    
    static var previews: some View {
        SidebarView(installationManager: _installationManager, devicesManager: _devicesManager)
        .listStyle(.sidebar)
    }
    
}
#endif
