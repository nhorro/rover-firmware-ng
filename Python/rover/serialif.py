import serial
import threading

from .endpointif import EndpointIF

class SerialIF(EndpointIF):
    def __init__(self, port, baudrate):
        self.port = port        
        self.baudrate = baudrate        
        self.serial = None
        self.thread = None
        self.running = False

    def set_callback(self, callback):
        self.callback = callback

    def start_listening(self):
        self.serial = serial.Serial(self.port, self.baudrate)
        self.running = True
        self.thread = threading.Thread(target=self._listen)
        self.thread.start()

    def stop_listening(self):
        self.running = False
        if self.thread:
            self.thread.join()
        if self.serial:
            self.serial.close()

    def send_data(self, data):
        if self.serial and self.serial.is_open:
            self.serial.write(data)

    def _listen(self):
        while self.running:
            if self.serial and self.serial.is_open and self.serial.in_waiting:
                data = self.serial.read()                
                self.callback(data)