import os
from .serialif import SerialIF
from struct import unpack
from . import simpleprotocol as sp
from .utils import bytearray_to_packet_str

from .tctm import telemetry as tmy
from .tctm import telecommands as tc

from .utils import csv_header_from_packet


class TelemetryCSVLogger:
    def __init__(self,csv_header):
        self.fp = None
        self.csv_header = csv_header
        pass
    
    def start(self,filename):
        self.fp = open(filename, "w")
        self.fp.write(self.csv_header+"\n")
        
    def stop(self):
        if self.fp:
            self.fp.close()
            self.fp = None
    
    def process(self, packet):
        if self.fp:
            self.fp.write("".join( ["{},".format(v) for k,v in packet.get_field_values().items()] )[:-1]+"\n")

class VehicleIF(SerialIF):
    MOTOR_MODE_DISARM = 0
    MOTOR_MODE_ARM_MANUAL = 1
    MOTOR_MODE_ARM_PID = 2

    """ Vehicle interface using serial port.
    """
    def __init__(self, port, baudrate,debug=False):
        SerialIF.__init__(self, port, baudrate, self._on_data)
        self.debug = debug
        self.connection_errors = 0
        self.capture_path = "."
        self.decoder = sp.Decoder(
            user_handler_fn=self._handle_packet, 
            user_error_handler_fn=self._handle_error)
        self.encoder = sp.Encoder()
        self.start_listening()
        
        self.telemetry_report_handlers = {
           0: self._handle_general_telemetry,
           1: self._handle_motor_control_telemetry
        }
        
        self.last_general_telemetry = None
        self.last_motor_control_telemetry = None
        
        self.telemetry_loggers = {
            tmy.GeneralTelemetry: TelemetryCSVLogger(csv_header_from_packet(tmy.GeneralTelemetry)),
            tmy.MotorControlTelemetry: TelemetryCSVLogger(csv_header_from_packet(tmy.MotorControlTelemetry))
        }

        self.telemetry_formatters = {
            "GeneralStatus":  lambda x: "0x{:08x}".format(x),
            "StatusFlags":  lambda x: "0x{:08x}".format(x),
            "Debug1":  lambda x: "0x{:08x}".format(x),
            "Debug2":  lambda x: "0x{:08x}".format(x)
        }

    def start_telemetry_csv_logging(self):
        for tm_type, tm_logger in self.telemetry_loggers.items():
            filename = str(tm_type)[14:-2]
            tm_logger.start(os.path.join(self.capture_path,f"{filename}.csv"))

    def stop_telemetry_csv_logging(self):
        for tm_type, tm_logger in self.telemetry_loggers.items():
            tm_logger.stop()
        
    def print_telemetry(self):
        print("General Telemetry")
        if self.last_general_telemetry:
            for name,value in self.last_general_telemetry.items():
                if name in self.telemetry_formatters:
                    formatted_value = self.telemetry_formatters[name](value)
                else:
                    formatted_value = value
                print("   {}: {}".format(name,formatted_value))
        else:
            print("No general telemetry frames have been received yet.")
        print("Motor Telemetry")
        if self.last_motor_control_telemetry:
            for name,value in self.last_motor_control_telemetry.items():
                if name in self.telemetry_formatters:
                    formatted_value = self.telemetry_formatters[name](value)
                else:
                    formatted_value = value
                print("   {}: {}".format(name,formatted_value))
        else:
            print("No motor control telemetry frames have been received yet.")

    def _handle_error(self, ec):
        self.connection_errors+=1
        if self.connection_errors > 3:
            print("Too many connection errors! Current error:", ec)

    def shutdown(self):
        self.stop_listening()
        
    def _on_data(self, c):
        self.decoder.feed(ord(c))
        
    def _send_command(self,cmd):
        self.encoder.encode(cmd.to_bytes())
        self.send_data(self.encoder.get_packet_bytes())
        #if self.debug:
        print("Sent: {} ({} bytes)".format(
            bytearray_to_packet_str(self.encoder.get_packet_bytes()),
            len(self.encoder.get_packet_bytes())        
        ))
                        
    def _handle_packet(self, payload):
        #print("Good packet!. Payload:", payload)        
        if self.debug:
            print("Received payload: {}".format(bytearray_to_packet_str(payload)))
            
        report_type = payload[0]
        if report_type in self.telemetry_report_handlers:
            tm = self.telemetry_report_handlers[report_type](payload[1:])
            if type(tm) in self.telemetry_loggers.keys():
                self.telemetry_loggers[type(tm)].process(tm)
        else:
            print("Unknown report type: 0x{:02x}".format(report_type))
        
    # =====================================================================
    # Commands here
    # =====================================================================
    
    # Commands    
    def control_led(self,led_state):
        cmd = tc.LedControlCommand()        
        cmd.control_flags = led_state
        self._send_command(cmd)
        
    def set_motor_throttles(self,a,b,flags):
        cmd = tc.ControlMotorManualCommand()        
        cmd.MotorControlFlags = flags
        cmd.MotorAThrottle = a
        cmd.MotorBThrottle = b
        self._send_command(cmd)
        
    def set_motor_speeds(self,a,b,flags):
        cmd = tc.ControlMotorAutoCommand()        
        cmd.MotorControlFlags = flags
        cmd.MotorASpeed = a
        cmd.MotorBSpeed = b
        self._send_command(cmd)
        
    def set_motor_control_mode(self,flags):
        cmd = tc.SetMotorControlModeCommand()        
        cmd.MotorControlModeFlags = flags        
        self._send_command(cmd)
        
    def set_pid_parameters(self,kp,ki,kd):
        cmd = tc.SetPIDParametersCommand()
        cmd.Kp = kp
        cmd.ki = ki
        cmd.kd = kd
        self._send_command(cmd)
        
    # =====================================================================
    # Telemetry report handling here
    # =====================================================================            
            
    def _handle_general_telemetry(self,payload):
        tm = tmy.GeneralTelemetry()
        tm.from_bytes(payload)
        self.last_general_telemetry = tm.get_field_values()
        return tm
        
    def _handle_motor_control_telemetry(self, payload):
        tm = tmy.MotorControlTelemetry()
        tm.from_bytes(payload)
        self.last_motor_control_telemetry = tm.get_field_values() 
        return tm
        