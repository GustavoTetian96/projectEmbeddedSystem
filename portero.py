import serial
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

Request = None
arduino = serial.Serial(
        port='/dev/ttyS1',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)
time.sleep(2)
class RequestHandler_httpd(BaseHTTPRequestHandler):
    def do_GET(self):
        global Request
        messagetosend = bytes('Solicitando conexion',"utf")
        self.send_response(200)
        self.send_header('Content-Type', 'text/plain')
        self.send_header('Content-Length', len(messagetosend))
        self.end_headers()
        self.wfile.write(messagetosend)
        Request = self.requestline
        Request = Request[5 : int(len(Request)-9)]
        print(Request)
        if Request == 'on1':
            print('abriendo puerta')
            arduino.write(str(1).encode('utf-8'))
        if Request == 'on2':
            print('cerrando  puerta')
            arduino.write(str(2).encode('utf-8'))
    


server_address_httpd = ('192.168.1.10',8080)
httpd = HTTPServer(server_address_httpd, RequestHandler_httpd)
print('conectando a servidor')
httpd.serve_forever()
