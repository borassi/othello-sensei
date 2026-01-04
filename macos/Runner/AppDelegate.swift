import Cocoa
import FlutterMacOS

@main
class AppDelegate: FlutterAppDelegate {
  var initialFile: String?

  override func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
    return true
  }
  // --- TRAP 1: Modern API (URLs) ---
  // This is the preferred method for macOS 10.13+
  @objc override func application(_ application: NSApplication, open urls: [URL]) {
    for url in urls {
      handlePath(url.path)
    }
  }

  // --- TRAP 2: Legacy API (Single File) ---
  // Some older mechanisms or simple "Open With" actions use this
  @objc override func application(_ sender: NSApplication, openFile filename: String) -> Bool {
    handlePath(filename)
    return true
  }

  // --- TRAP 3: Legacy API (Multiple Files) ---
  @objc override func application(_ sender: NSApplication, openFiles filenames: [String]) {
    for path in filenames {
      handlePath(path)
    }
  }

  // --- COMMON HANDLER ---
  func handlePath(_ path: String) {
    // Logic: Cache it and try to send it
    initialFile = path

    // If Flutter is already running (Warm Start), send it immediately via the channel
    if let controller = mainFlutterWindow?.contentViewController as? FlutterViewController {
       let channel = FlutterMethodChannel(name: "com.othellosensei.app/files", binaryMessenger: controller.engine.binaryMessenger)
       channel.invokeMethod("onFileOpened", arguments: path)
    }
  }

  override func applicationSupportsSecureRestorableState(_ app: NSApplication) -> Bool {
    return true
  }
}
