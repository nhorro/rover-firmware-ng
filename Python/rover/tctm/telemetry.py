from .. import simpleprotocol as sp
import ctypes
from struct import unpack, pack

class GeneralTelemetry(sp.BaseMessage):
    def __init__(self):        
        
        self.TelemetryCycle = 0
        self.OnBoardTime = 0
        self.ReceivedPackets = 0
        self.GeneralStatus = 0
        self.Debug1 = 0
        self.Debug2 = 0

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>IIIIii'
        sp.BaseMessage.__init__(self)
    
    def get_field_values(self):        
        result = {f: getattr(self, f) for f in self.fields }
        return result



class MotorControlTelemetry(sp.BaseMessage):
    def __init__(self):      

        self.TelemetryCycle = 0
        self.OnBoardTime = 0  
        
        self.Throttle1 = 0
        self.Throttle2 = 0
        
        self.Tachometer1 = 0
        self.Tachometer2 = 0
        self.Tachometer3 = 0
        self.Tachometer4 = 0
        
        self.MeasuredSpeed1 = 0
        self.MeasuredSpeed2 = 0
        self.MeasuredSpeed3 = 0
        self.MeasuredSpeed4 = 0
        
        self.SetpointSpeed1 = 0
        self.SetpointSpeed2 = 0
        
        self.StatusFlags = 0

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>IIffIIIIffffffI'
        sp.BaseMessage.__init__(self)
    
    def get_field_values(self):        
        result = {f: getattr(self, f) for f in self.fields }
        return result        


class IMUTelemetry(sp.BaseMessage):
    def __init__(self):      

        self.TelemetryCycle = 0
        self.OnBoardTime = 0  
        
        self.Roll = 0.
        self.Pitch = 0.
        self.Yaw = 0.
        
        # see: https://docs.python.org/3/library/struct.html
        self.format = '>IIfff'
        sp.BaseMessage.__init__(self)
    
    def get_field_values(self):        
        result = {f: getattr(self, f) for f in self.fields }
        return result            