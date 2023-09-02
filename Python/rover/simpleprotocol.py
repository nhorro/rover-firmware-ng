"""
Functions to handle a simple package serialization protocol.


- Header (2 bytes): 0xEB90
- Length (2 bytes): Length of the payload in bytes.
- Payload ("Length" bytes): 
- CRC16 (2 bytes): CRC-16/CCITT-FALSE of the packet payload.

"""
import time
from struct import unpack, pack

HEADER_SIZE = 2+1
PAYLOAD_BUFFER_SIZE =  256
TRAILER_SIZE = 1+1
PACKET_TIMEOUT_IN_MS =1000
MAX_PACKET_SIZE = HEADER_SIZE + PAYLOAD_BUFFER_SIZE + TRAILER_SIZE

PACKET_SYNC_0_CHAR = ord('@')
PACKET_SYNC_1_CHAR = ord('<')
PACKET_TERMINATOR_CHAR = ord('\n')


class Encoder:
    def __init__(self, debug=False):
        self.packet_bytes = None
        self.payload_length = 0
        self.debug=debug
        
    def encode(self, payload:bytearray):
        self.packet_bytes = bytearray()
        self.packet_bytes.append(PACKET_SYNC_0_CHAR)
        self.packet_bytes.append(PACKET_SYNC_1_CHAR)
        self.payload_length = len(payload)
        self.packet_bytes.append(self.payload_length)
        self.packet_bytes.extend( [x for x in payload])
        crc = _calculate_crc8(self.packet_bytes[3:])    
        self.packet_bytes.append(crc)
        self.packet_bytes.append(PACKET_TERMINATOR_CHAR)

    def get_packet_bytes(self):
        return self.packet_bytes
    
    def get_payload_length(self):
        return self.payload_length
    

class ErrorCode:
    success = 0
    bad_sync = 1
    invalid_length = 2
    bad_crc = 3
    bad_terminator = 4
    timeout = 5


class Decoder:
    pkt_state_idle = 0
    pkt_state_expecting_start_sync1 = 1
    pkt_state_expecting_length = 2
    pkt_state_expecting_payload = 3
    pkt_state_expecting_crc = 4
    pkt_state_expecting_terminator = 5

    def __init__(self, user_handler_fn, user_error_handler_fn, debug=False):        
        self.packet_state_handlers = {
            Decoder.pkt_state_idle: self._handle_pkt_state_idle,
	        Decoder.pkt_state_expecting_start_sync1: self._handle_pkt_state_expecting_start_sync1,
	        Decoder.pkt_state_expecting_length: self._handle_pkt_state_expecting_length,
            Decoder.pkt_state_expecting_payload: self._handle_pkt_state_expecting_payload,
	        Decoder.pkt_state_expecting_crc: self._handle_pkt_state_expecting_crc,
            Decoder.pkt_state_expecting_terminator: self._handle_pkt_state_expecting_terminator
        }
        self.last_error_code = None
        self.reset()
        self.user_handler_fn = user_handler_fn        
        self.user_error_handler_fn = user_error_handler_fn
        self.debug = debug

    def reset(self):
        self.current_state = Decoder.pkt_state_idle
        self.last_received_char = None
        self.payload_length = 0
        self.received_payload_index = 0
        self.start_of_packet_t0 = _get_current_time_ms()        

    def feed(self, c):                
        if self.debug:
            print("State: {} S: 0x{:02x}".format(self.current_state,ord(c)))
        self.last_received_char = c        
        self.packet_state_handlers[self.current_state]()
        return self.last_error_code

    def _handle_pkt_state_idle(self):        
        if PACKET_SYNC_0_CHAR == self.last_received_char:
            self.current_state = Decoder.pkt_state_expecting_start_sync1
        #else:
        #    print("Spurius character: ", self.last_received_char)
         
    def _handle_pkt_state_expecting_start_sync1(self):        
        if PACKET_SYNC_1_CHAR == self.last_received_char:
            self.current_state = Decoder.pkt_state_expecting_length
        else:
            self.set_error(ErrorCode.bad_sync)
            self.reset()
    
    def _handle_pkt_state_expecting_length(self):        
        self.payload_length = self.last_received_char        
        if self.debug:
            print(f"Payload length: {self.payload_length} bytes.")
        if self.payload_length and (self.payload_length <= PAYLOAD_BUFFER_SIZE ):
            self.current_state = Decoder.pkt_state_expecting_payload
            self.received_payload_buffer = bytearray()
        else:
            self.set_error(ErrorCode.invalid_length)
            self.reset()

    def _handle_pkt_state_expecting_payload(self):        
        self.received_payload_buffer.append(self.last_received_char)
        self.received_payload_index+=1        
        if self.received_payload_index == self.payload_length:
            self.expected_crc8 = _calculate_crc8(self.received_payload_buffer)
            self.current_state = Decoder.pkt_state_expecting_crc        

    def _handle_pkt_state_expecting_crc(self):        
        crc8 = self.last_received_char
        #print("Expected CRC: 0x{:02x}. Found CRC: 0x{:02x}".format(self.expected_crc8, crc8))
        if crc8 == self.expected_crc8:
            self.current_state = Decoder.pkt_state_expecting_terminator
        else:
            self.set_error(ErrorCode.bad_crc)
            self.reset()

    def _handle_pkt_state_expecting_terminator(self):
        if PACKET_TERMINATOR_CHAR == self.last_received_char:
            self.set_error(ErrorCode.success)
            self.last_received_packet_t0 = _get_current_time_ms()
            self.user_handler_fn(self.received_payload_buffer)
        else:
            self.set_error(ErrorCode.bad_terminator)
        self.reset()

    def set_error(self, error_code:ErrorCode):        
        self.last_error_code = error_code
        if error_code != ErrorCode.success:
            self.user_error_handler_fn(self.last_error_code)


def _calculate_crc8(data : bytearray) -> int:
    """ 
    Computes the CRC-16/CCITT-FALSE of a byte sequence.
    Arguments:
        data: bytearray object with the packet.
    Returns:
       CRC8
    """        
    if data is None:
        return 0
    length = len(data)    
    crc = 0xff
    for i in range(0, length):
        crc ^= data[i]
        for j in range(0,8):
            if (crc & 0x80) > 0:
                crc =(crc << 1) ^ 0x31
            else:
                crc = crc << 1
    return crc & 0xFF


def _get_current_time_ms():
    return round(time.time() * 1000)

class BaseMessage:
    def __init__(self):
        self.fields = [attr for attr in self.__dict__.keys() if not callable(getattr(self, attr)) and not attr.startswith("__")]
        assert(self.format) # format field must be defined
        self.fields.remove("format")

    def get_fields(self):
        return self.fields

    def from_bytes(self, data):
        values = unpack(self.format, data)
        for idx, m in enumerate(self.fields):
            setattr(self, m, values[idx])

    def to_bytes(self):
        values = [getattr(self, f) for f in self.fields]
        return pack(self.format, *values)
    
    def __repr__(self):
        result = str({f: getattr(self, f) for f in self.fields })
        return result