import socket
import threading

from .endpointif import EndpointIF

class UDPProxy(EndpointIF):
    def __init__(self, publish_host, publish_port, subscribe_port):        
        self.publish_host = publish_host
        self.publish_port = publish_port
        self.subscribe_port = subscribe_port
        self.running = False

    def set_callback(self, callback):
        self.callback = callback

    def start_listening(self):
        self.publisher = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.subscriber = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Bind to the local address and port for receiving        
        self.subscriber.bind(("0.0.0.0", self.subscribe_port))

        self.running = True

        # Start a dedicated thread for receiving data
        self.receive_thread = threading.Thread(target=self._listen)
        self.receive_thread.start()

    def stop_listening(self):
        self.running = False
        self.receive_thread.join()
        self.publisher.close()
        self.subscriber.close()

    def send_data(self, data):
        print(f"Sending to host {self.publish_host}, port: {self.publish_port}")
        self.publisher.sendto(data, (self.publish_host, self.publish_port))

    def _listen(self):
        # Set a timeout for receiving data (in seconds)                         
        self.subscriber.settimeout(0.5)  # Adjust the timeout as needed
        while self.running:            
            try:      
                data, _ = self.subscriber.recvfrom(1024)  # Adjust buffer size as needed                
                if self.callback:
                    self.callback(data)
            except socket.timeout:
                continue
            except Exception as e:
                print(f"Error while receiving data: {e}")