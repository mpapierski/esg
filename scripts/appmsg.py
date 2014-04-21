"""
Fake "appmsg" server.

Add this line to /etc/hosts:
127.0.0.1 appmsg.gadu-gadu.pl
"""
import os
from urlparse import urlparse, parse_qs

import SocketServer
import SimpleHTTPServer


class CustomHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    def do_GET(self):
        o = urlparse(self.path)
        if (o.path == '/appsvc/appmsg_ver8.asp'
                or o.path == '/appsvc/appmsg_ver11.asp'):
            qs = parse_qs(o.query)
            self.send_response(200)
            self.end_headers()
            self.wfile.write('0 0 127.0.0.1:8074 127.0.0.1')
        else:
            self.send_response(404)
            self.send_header('Content-type', 'text/html')
            self.wfile.write('Not found')

class HTTPServer(SocketServer.TCPServer):
    allow_reuse_address = True

server = HTTPServer(('localhost', 80), CustomHandler)
server.serve_forever()
