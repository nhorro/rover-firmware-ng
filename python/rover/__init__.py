from .serialprotocol import Connection, build_packet
import time
import struct

DEFAULT_ROVER_PORT = '/dev/ttyACM0'
DEFAULT_ROVER_BAUDRATE = 115200

class RoverClient(Connection):        
    """ Cliente para conectarse con Rover.
    """
    
    # --- Telecomandos ----

    CMD_REQUEST_TMY = 0    
    CMD_LED_ON = 1    
    CMD_LED_OFF = 2    
    CMD_UPDATE_MOTOR_THROTTLES = 3
    CMD_UPDATE_MOTOR_SPEED_SETPOINTS = 4
    CMD_CONFIGURE_PID = 5
    CMD_REQUEST_DEBUG_REPORT = 6

    # Reportes

    # Reporte general de telemetría. 
    REPORT_GENERAL_TELEMETRY = 0x80

    # Reporte de resultado de ejecución de un comando.
    REPORT_COMMAND_EXECUTION_STATUS = 0x81

    # Reporte con estado de IMU. 
    REPORT_IMU_AHRS_STATE = 0x82

    # Reporte con de movimiento (tacómetros y motores). 
    REPORT_MOTION_CONTROL_STATE = 0x83

    # Reporte de posición (GPS). 
    REPORT_GPS_STATE = 0x84

    # Reporte con parámetros de PID
    REPORT_PID_PARAMS = 0x85

    # Enums
    MOTOR_A = 0x01
    MOTOR_B = 0x02


    # Telemetría del Rover

    # Contador de paquetes aceptados.
    TMY_PARAM_ACCEPTED_PACKETS = 0

    # Contador de paquetes rechazados. 
    TMY_PARAM_REJECTED_PACKETS = 0

    # Código de último comando ejecutado. 
    TMY_PARAM_LAST_OPCODE = 0

    # Código de último error/status. 
    TMY_PARAM_LAST_ERROR = 0
    TMY_PARAM_STATUS = 0

    # Telemetría de IMU
    
    # Eje X de Acelerómetro (ingeniería)
    TMY_PARAM_IMU_ENG_ACCEL_X = 0

    # Eje Y de Acelerómetro (ingeniería) */
    TMY_PARAM_IMU_ENG_ACCEL_Y = 0

    # Eje Z de Acelerómetro (ingeniería) */
    TMY_PARAM_IMU_ENG_ACCEL_Z = 0

    # Eje X de Giróscopo (ingeniería) */
    TMY_PARAM_IMU_ENG_GYRO_X = 0

    # Eje Y de Giróscopo (ingeniería) */
    TMY_PARAM_IMU_ENG_GYRO_Y = 0

    # Eje Z de Giróscopo (ingeniería) */
    TMY_PARAM_IMU_ENG_GYRO_Z = 0

    # Eje X de Magnetómetro (ingeniería) */
    TMY_PARAM_IMU_ENG_MAG_X = 0

    # Eje Y de Magnetómetro (ingeniería) */
    TMY_PARAM_IMU_ENG_MAG_Y = 0

    # Eje Z de Magnetómetro (ingeniería) */
    TMY_PARAM_IMU_ENG_MAG_Z = 0

    # Temperatura */
    TMY_PARAM_IMU_TEMP = 0

    # IMU (Estimaciones)

    # Componente X de cuaternion estimado (Kalman o Madwick) */
    TMY_PARAM_IMU_QUAT_X = 0

    # Componente Y de cuaternion estimado (Kalman o Madwick) */
    TMY_PARAM_IMU_QUAT_Y = 0

    # Componente Z de cuaternion estimado (Kalman o Madwick) */
    TMY_PARAM_IMU_QUAT_Z = 0

    # Componente W de cuaternion estimado (Kalman o Madwick) */
    TMY_PARAM_IMU_QUAT_W = 0

    # Velocidad medida en tacómetros (RPM)
    TMY_PARAM_TACHO1_SPEED = 0
    TMY_PARAM_TACHO2_SPEED = 0
    TMY_PARAM_TACHO3_SPEED = 0
    TMY_PARAM_TACHO4_SPEED = 0

    # Cuentas en tacómetros 
    TMY_PARAM_TACHO1_COUNT = 0
    TMY_PARAM_TACHO2_COUNT = 0
    TMY_PARAM_TACHO3_COUNT = 0
    TMY_PARAM_TACHO4_COUNT = 0

    # Control de motores (velocidad para cada par)
    TMY_PARAM_MOTOR_A_THROTTLE = 0
    TMY_PARAM_MOTOR_B_THROTTLE = 0

    # Control de motores (PID)
    
    # Velocidad establecida (setpoint) (RPM)
    TMY_PARAM_MOTOR_A_SETPOINT_SPEED = 0
    TMY_PARAM_MOTOR_B_SETPOINT_SPEED = 0

    def __init__(self, port=DEFAULT_ROVER_PORT, baudrate=DEFAULT_ROVER_BAUDRATE):  
        """ Constructor por defecto.
        
        Args
        ----

            port(str): puerto de conexión con Rover (en x86 Linux suele ser '/dev/ttyACM0')
            baudrate(int): baudrate (por defecto es 115200)
        """
        Connection.__init__(self,port, baudrate,  self.__on_data_received)

        self.__report_handlers = {
            RoverClient.REPORT_GENERAL_TELEMETRY: self.__on_general_tmy_report,
            RoverClient.REPORT_COMMAND_EXECUTION_STATUS: self.__on_command_execution_status_report,
            RoverClient.REPORT_IMU_AHRS_STATE: self.__on_imu_status_report,
            RoverClient.REPORT_MOTION_CONTROL_STATE: self.__on_motion_control_state_report,
            RoverClient.REPORT_GPS_STATE: self.__on_gps_report
        }

        self.__report_counters = {
            "GENERAL_TELEMETRY": 0,
            "COMMAND_EXECUTION_STATUS": 0,
            "IMU_AHRS_STATE": 0,
            "MOTION_CONTROL_STATE": 0,
            "GPS_STATE": 0,
            "INVALID": 0
        }

        self.connect()

    # API de comandos

    def request_tmy(self, report_type=0):
        """ Pedido de telemetría.
        """
        pkt = build_packet([
            RoverClient.CMD_REQUEST_TMY,
            report_type
            ])
        self.send_packet(pkt)

    def led_on(self):
        """ Encender led de prueba.
        """
        pkt = build_packet([RoverClient.CMD_LED_ON])
        self.send_packet(pkt)
                
    def led_off(self):
        """ Apagar led de prueba.
        """
        pkt = build_packet([RoverClient.CMD_LED_OFF])
        self.send_packet(pkt)

    def set_motor_throttles(self, throttles, flags ):        
        """ Establecer potencia de motores (en modo manual)
            
        Args
        ----
            speeds(list): velocidad de cada par entre (-1f,1f)
        """
        packed = struct.pack('<2f',throttles[0],throttles[1])
        pkt = build_packet(
            [
                RoverClient.CMD_UPDATE_MOTOR_THROTTLES,
                packed[0],packed[1],packed[2],packed[3],
                packed[4],packed[5],packed[6],packed[7],
                flags
            ] )
        """
        tmp_str = ""
        for x in pkt:
            tmp_str += "%02X " % x
        print(tmp_str)
        """
        self.send_packet(pkt)


    def set_motor_speed_setpoint(self, setpoint_speeds, flags ):        
        """ Establecer setpoint de velocidad de motores
            
        Args
        ----
            speeds(list): velocidad en RPM (floats)
        """
        packed = struct.pack('<2f',setpoint_speeds[0],setpoint_speeds[1])
        pkt = build_packet(
            [
                RoverClient.CMD_UPDATE_MOTOR_SPEED_SETPOINTS,
                packed[0],packed[1],packed[2],packed[3],
                packed[4],packed[5],packed[6],packed[7],
                flags
            ] )
        """
        tmp_str = ""
        for x in pkt:
            tmp_str += "%02X " % x
        print(tmp_str)
        """
        self.send_packet(pkt)      

    def configure_motor_pid(self, kp,kd,ki ):        
        """ Establecer parámetros de PID (se usan para los dos motores)
            
        Args
        ----
            kp(float): Proporcional.
            kd(float): Derivativo.
            ki(float): Integral.
        """
        packed = struct.pack('<3f',kp,kd,ki)
        pkt = build_packet(
            [
                RoverClient.CMD_CONFIGURE_PID,
                packed[0],packed[1],packed[2],packed[3],
                packed[4],packed[5],packed[6],packed[7],
                packed[8],packed[9],packed[10],packed[11]
            ] )
        """
        tmp_str = ""
        for x in pkt:
            tmp_str += "%02X " % x
        print(tmp_str)
        """
        self.send_packet(pkt)                  

    def request_debug_report(self):
        """ 
        """        
        pkt = build_packet([RoverClient.CMD_REQUEST_DEBUG_REPORT] )
        self.send_packet(pkt)   


    def __on_data_received(self, payload):
        """ Manejar las respuestas del Rover
        """
        report_code = payload[0]

        if report_code in self.__report_handlers:
            try:
                self.__report_handlers[report_code](payload[1:])
            except IndexError as ex:
                print("Error processing report type {}".format(report_code))
                raise(ex)
        else:
            self.__report_counters["INVALID"]+=1   
            print("Reporte desconocido:", payload)
            #tmp = ""
            #for x in payload:
            #    tmp+="%02X " % x
            #print(tmp)

    def __on_general_tmy_report(self,payload):
        """ Procesar un reporte de telemetría general.
        """        
        self.__report_counters["GENERAL_TELEMETRY"]+=1   

        # Telemetría general
        self.TMY_PARAM_ACCEPTED_PACKETS = payload[0]
        self.TMY_PARAM_REJECTED_PACKETS = payload[1]
        self.TMY_PARAM_LAST_OPCODE = payload[2]
        self.TMY_PARAM_LAST_ERROR = payload[3]
        self.TMY_PARAM_STATUS = payload[4]

    def __on_command_execution_status_report(self,payload):
        """ Procesar un reporte de ejecución.
        """
        self.__report_counters["COMMAND_EXECUTION_STATUS"]+=1
        
    def __on_motion_control_state_report(self,payload):
        """ Procesar un reporte de estado de motores y tacómetros.
        """
        self.__report_counters["MOTION_CONTROL_STATE"]+=1

        parsed = struct.unpack('<ccc4f4I2f2f', bytearray(payload))
        self.TMY_PARAM_TACHO1_SPEED = parsed[3]
        self.TMY_PARAM_TACHO2_SPEED = parsed[4]
        self.TMY_PARAM_TACHO3_SPEED = parsed[5]
        self.TMY_PARAM_TACHO4_SPEED = parsed[6]

        self.TMY_PARAM_TACHO1_COUNT = parsed[7]
        self.TMY_PARAM_TACHO2_COUNT = parsed[8]
        self.TMY_PARAM_TACHO3_COUNT = parsed[9]
        self.TMY_PARAM_TACHO4_COUNT = parsed[10]

        self.TMY_PARAM_MOTOR_A_THROTTLE = parsed[11]
        self.TMY_PARAM_MOTOR_B_THROTTLE = parsed[12]

        self.TMY_PARAM_MOTOR_A_SETPOINT_SPEED = parsed[13]
        self.TMY_PARAM_MOTOR_B_SETPOINT_SPEED = parsed[14]

    def __on_gps_report(self,payload):
        """ Procesar un reporte de GPS.
        """
        self.__report_counters["GPS_STATE"]+=1

    def __on_imu_status_report(self,payload):
        """ Procesar un reporte de IMU.
        """
        self.__report_counters["IMU_AHRS_STATE"]+=1

        parsed = struct.unpack('<ccc14f', bytearray(payload))
        
        self.TMY_PARAM_IMU_ENG_ACCEL_X = parsed[3]
        self.TMY_PARAM_IMU_ENG_ACCEL_Y = parsed[4] 
        self.TMY_PARAM_IMU_ENG_ACCEL_Z = parsed[5]
        self.TMY_PARAM_IMU_ENG_GYRO_X = parsed[6]
        self.TMY_PARAM_IMU_ENG_GYRO_Y = parsed[7]
        self.TMY_PARAM_IMU_ENG_GYRO_Z = parsed[8]
        self.TMY_PARAM_IMU_ENG_MAG_X = parsed[9]
        self.TMY_PARAM_IMU_ENG_MAG_Y = parsed[10]
        self.TMY_PARAM_IMU_ENG_MAG_Z = parsed[11] 
        self.TMY_PARAM_IMU_TEMP = parsed[12] 

        # IMU (Estimaciones)
        self.TMY_PARAM_IMU_QUAT_X = parsed[13]
        self.TMY_PARAM_IMU_QUAT_Y = parsed[14]
        self.TMY_PARAM_IMU_QUAT_Z = parsed[15]
        self.TMY_PARAM_IMU_QUAT_W = parsed[16]

    def print_general_tmy(self):
        print("ACCEPTED_PACKETS: {}".format(self.TMY_PARAM_ACCEPTED_PACKETS))
        print("REJECTED_PACKETS: {}".format(self.TMY_PARAM_REJECTED_PACKETS))
        print("LAST_OPCODE: 0x{:02x}".format(self.TMY_PARAM_LAST_OPCODE))
        print("LAST_ERROR: 0x{:02x}".format(self.TMY_PARAM_LAST_ERROR))
        print("STATUS: 0x{:02x}".format(self.TMY_PARAM_STATUS))

    def print_motor_tmy(self):
        print("TACHO1_SPEED: {:.3f}".format(self.TMY_PARAM_TACHO1_SPEED))
        print("TACHO2_SPEED: {:.3f}".format(self.TMY_PARAM_TACHO2_SPEED))
        print("TACHO3_SPEED: {:.3f}".format(self.TMY_PARAM_TACHO3_SPEED))
        print("TACHO4_SPEED: {:.3f}".format(self.TMY_PARAM_TACHO4_SPEED))

        print("TACHO1_COUNT: {:.3f}".format(self.TMY_PARAM_TACHO1_COUNT))
        print("TACHO2_COUNT: {:.3f}".format(self.TMY_PARAM_TACHO2_COUNT))
        print("TACHO3_COUNT: {:.3f}".format(self.TMY_PARAM_TACHO3_COUNT))
        print("TACHO4_COUNT: {:.3f}".format(self.TMY_PARAM_TACHO4_COUNT))

        print("MOTOR_A_THROTTLE: {:}".format(self.TMY_PARAM_MOTOR_A_THROTTLE))
        print("MOTOR_B_THROTTLE: {:}".format(self.TMY_PARAM_MOTOR_B_THROTTLE))

        print("MOTOR_A_SETPOINT_SPEED: {:.3f}".format(self.TMY_PARAM_MOTOR_A_SETPOINT_SPEED))
        print("MOTOR_B_SETPOINT_SPEED: {:.3f}".format(self.TMY_PARAM_MOTOR_B_SETPOINT_SPEED))


    def print_imu_state(self):
        print("IMU_ENG_ACCEL_X: {:.3f}".format(self.TMY_PARAM_IMU_ENG_ACCEL_X))
        print("IMU_ENG_ACCEL_Y: {:.3f}".format(self.TMY_PARAM_IMU_ENG_ACCEL_Y))
        print("IMU_ENG_ACCEL_Z: {:.3f}".format(self.TMY_PARAM_IMU_ENG_ACCEL_Z))

        print("IMU_ENG_GYRO_X: {:.3f}".format(self.TMY_PARAM_IMU_ENG_GYRO_X))
        print("IMU_ENG_GYRO_Y: {:.3f}".format(self.TMY_PARAM_IMU_ENG_GYRO_Y))
        print("IMU_ENG_GYRO_Z: {:.3f}".format(self.TMY_PARAM_IMU_ENG_GYRO_Z))

        print("IMU_ENG_MAG_X: {:.3f}".format(self.TMY_PARAM_IMU_ENG_MAG_X))
        print("IMU_ENG_MAG_Y: {:.3f}".format(self.TMY_PARAM_IMU_ENG_MAG_Y))
        print("IMU_ENG_MAG_Z: {:.3f}".format(self.TMY_PARAM_IMU_ENG_MAG_Z))

        print("IMU_TEMP: {:.3f}".format(self.TMY_PARAM_IMU_TEMP))
        
        print("IMU_QUAT_X: {:.3f}".format(self.TMY_PARAM_IMU_QUAT_X))
        print("IMU_QUAT_Y: {:.3f}".format(self.TMY_PARAM_IMU_QUAT_Y))
        print("IMU_QUAT_Z: {:.3f}".format(self.TMY_PARAM_IMU_QUAT_Z))
        print("IMU_QUAT_W: {:.3f}".format(self.TMY_PARAM_IMU_QUAT_W))


    def get_report_counts(self):
        return self.__report_counters