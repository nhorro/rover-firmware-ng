from .. import simpleprotocol as sp
import ctypes
from struct import unpack, pack


class LedControlCommand(sp.BaseMessage):
    def __init__(self):
        # Header
        self.opcode = 0
        
        # Payload
        self.control_flags = 0

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>BI'
        sp.BaseMessage.__init__(self)



class ControlMotorManualCommand(sp.BaseMessage):
    def __init__(self):
        # Header
        self.opcode = 1
        
        # Payload
        self.MotorControlFlags = 0
        self.MotorAThrottle = 0.
        self.MotorBThrottle = 0.

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>BIff'
        sp.BaseMessage.__init__(self)        



class ControlMotorAutoCommand(sp.BaseMessage):
    def __init__(self):
        # Header
        self.opcode = 2
        
        # Payload
        self.MotorControlFlags = 0
        self.MotorASpeed = 0.
        self.MotorBSpeed = 0.

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>BIff'
        sp.BaseMessage.__init__(self)



class SetMotorControlModeCommand(sp.BaseMessage):
    def __init__(self):
        # Header
        self.opcode = 3
        
        # Payload
        self.MotorControlModeFlags = 0

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>BI'
        sp.BaseMessage.__init__(self)        



class SetPIDParametersCommand(sp.BaseMessage):
    def __init__(self):
        # Header
        self.opcode = 4
        
        # Payload
        self.Kp = 0.0
        self.Ki = 0.0
        self.Kd = 0.0

        # see: https://docs.python.org/3/library/struct.html
        self.format = '>Bfff'
        sp.BaseMessage.__init__(self)        