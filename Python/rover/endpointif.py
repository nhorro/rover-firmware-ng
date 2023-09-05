from __future__ import annotations
from abc import ABC, abstractmethod

class EndpointIF(ABC):
    @abstractmethod
    def set_callback(self, callback):
        pass

    @abstractmethod
    def start_listening(self):
        pass

    @abstractmethod
    def stop_listening(self):
        pass

    @abstractmethod
    def send_data(self, data):
        pass