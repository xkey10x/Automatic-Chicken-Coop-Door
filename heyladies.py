import machine
import utime
from machine import Pin

# Initialize stepper motor
motor_coil_A_1_pin = Pin(0, Pin.OUT)
motor_coil_A_2_pin = Pin(2, Pin.OUT)
motor_coil_B_1_pin = Pin(4, Pin.OUT)
motor_coil_B_2_pin = Pin(5, Pin.OUT)
motor_coil_A_1_pin.off()
motor_coil_A_2_pin.off()
motor_coil_B_1_pin.off()
motor_coil_B_2_pin.off()
motor_sequence = [[1, 0, 0, 1], [1, 0, 0, 0], [1, 1, 0, 0], [0, 1, 0, 0], [0, 1, 1, 0], [0, 0, 1, 0], [0, 0, 1, 1], [0, 0, 0, 1]]

# Initialize hall effect sensors
hall_sensor_1_pin = Pin(12, Pin.IN, Pin.PULL_UP)
hall_sensor_2_pin = Pin(14, Pin.IN, Pin.PULL_UP)

# Initialize light sensor
ldr_pin = machine.ADC(0)

# Define motor control function
def move_motor(steps, direction):
    for i in range(steps):
        for step in range(8):
            motor_coil_A_1_pin.value(motor_sequence[step][0])
            motor_coil_A_2_pin.value(motor_sequence[step][1])
            motor_coil_B_1_pin.value(motor_sequence[step][2])
            motor_coil_B_2_pin.value(motor_sequence[step][3])
            utime.sleep_ms(2)
    if direction == 'cw':
        while not hall_sensor_2_pin.value():
            motor_coil_A_1_pin.value(motor_sequence[0][0])
            motor_coil_A_2_pin.value(motor_sequence[0][1])
            motor_coil_B_1_pin.value(motor_sequence[0][2])
            motor_coil_B_2_pin.value(motor_sequence[0][3])
            utime.sleep_ms(2)
    else:
        while not hall_sensor_1_pin.value():
            motor_coil_A_1_pin.value(motor_sequence[0][0])
            motor_coil_A_2_pin.value(motor_sequence[0][1])
            motor_coil_B_1_pin.value(motor_sequence[0][2])
            motor_coil_B_2_pin.value(motor_sequence[0][3])
            utime.sleep_ms(2)

# Set initial state of door
door_state = 'closed'

while True:
    # Check light sensor
    light_level = ldr_pin.read()
    if light_level > 2000 and door_state == 'closed': # Daytime and door closed
        move_motor(800, 'ccw') # Open door
        door_state = 'open'
    elif light_level < 2000 and door_state == 'open': # Nighttime and door open
        move_motor(800, 'cw') # Close door
        door_state = 'closed'
    utime.sleep(60) # Wait 1 minute before checking light sensor again
