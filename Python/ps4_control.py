import pygame
import asyncio

from rover.vehicleif import VehicleIF


async def ps4_controller_listener(serial_port, control_send_frequency):
    keep_running = True

    pygame.init()
    pygame.joystick.init()

    # Ensure a PS4 controller is connected
    if pygame.joystick.get_count() < 1:
        print("No PS4 controller found.")
    else:
        controller = pygame.joystick.Joystick(0)
        controller.init()  


    vehicle = VehicleIF(serial_port, 115200,debug=False)
    vehicle.capture_path = "./captures"

    is_armed = False
    throttle = [ 0.0, 0.0 ]
    while keep_running:        
        for event in pygame.event.get():            
            if event.type == pygame.KEYDOWN:
                keys = pygame.key.get_pressed()
                if keys[pygame.K_ESCAPE]:
                    keep_running = False
                    
            if event.type == pygame.QUIT:
                keep_running = False

            if event.type == pygame.JOYAXISMOTION:                                        
                if event.axis == 1:
                    throttle[0] = -round(event.value,2)
                elif event.axis == 4:
                    throttle[1] = -round(event.value,2)                    
                if is_armed:
                    vehicle.set_motor_throttles(throttle[0],throttle[1],0x3)
                    #print("Throttle1: {} Throttle2: {}".format(throttle[0],throttle[1]))                                    
            #elif event.type == pygame.JOYBUTTONDOWN:                    
            elif event.type == pygame.JOYBUTTONUP:                    
                if event.button == 1:
                    is_armed = not is_armed
                    print("IS_ARMED:",is_armed)       
                    if is_armed:
                        vehicle.set_motor_control_mode(VehicleIF.MOTOR_MODE_ARM_MANUAL)
                    else:                 
                        vehicle.set_motor_control_mode(VehicleIF.MOTOR_MODE_DISARM)
                    #self.toggle_armed()
                    
        await asyncio.sleep(1.0/control_send_frequency)  # Adjust sleep duration as needed

if __name__ == "__main__":      
    serial_port="/dev/ttyACM0"
    control_send_frequency_in_hz = 5
    asyncio.run(ps4_controller_listener(serial_port, control_send_frequency_in_hz))