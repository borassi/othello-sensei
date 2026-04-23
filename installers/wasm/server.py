

import os
from http.server import HTTPServer, SimpleHTTPRequestHandler

os.chdir('installers/wasm')

class WASMHandler(SimpleHTTPRequestHandler):
  def end_headers(self):
    self.send_header("Cross-Origin-Opener-Policy", "same-origin")
    self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
    self.send_header("Cache-Control", "no-store, must-revalidate")
    super().end_headers()

print("Serving on http://localhost:8080")
HTTPServer(('localhost', 8080), WASMHandler).serve_forever()