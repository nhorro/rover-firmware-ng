import zmq
import threading

from .endpointif import EndpointIF

class ZMQProxy(EndpointIF):
    def __init__(self, publish_port, subscribe_host, subscribe_port):
        self.publish_port = publish_port
        self.subscribe_host = subscribe_host
        self.subscribe_port = subscribe_port        
        self.running = False

    def set_callback(self,callback):
        self.callback = callback

    def start_listening(self):
        self.context = zmq.Context()
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.bind(f"tcp://*:{self.publish_port}")

        self.subscriber = self.context.socket(zmq.SUB)
        self.subscriber.connect(f"tcp://{self.subscribe_host}:{self.subscribe_port}")
        self.subscriber.setsockopt_string(zmq.SUBSCRIBE, "")

        self.running = True

        # Start a dedicated thread for receiving data
        self.receive_thread = threading.Thread(target=self._listen)
        self.receive_thread.start()

    def stop_listening(self):
        self.running = False
        self.receive_thread.join()
        self.publisher.close()
        self.subscriber.close()
        self.context.term()

    def send_data(self, data):
        if self.running:
            self.publisher.send(data)

    def _listen(self):
        poller = zmq.Poller()
        poller.register(self.subscriber, zmq.POLLIN)

        while self.running:
            socks = dict(poller.poll(timeout=500))  # 0.5-second timeout

            if self.subscriber in socks and socks[self.subscriber] == zmq.POLLIN:
                try:
                    data = self.subscriber.recv()
                    if self.callback:
                        self.callback(data)
                except zmq.ZMQError as e:
                    if e.errno == zmq.ETERM:
                        break
