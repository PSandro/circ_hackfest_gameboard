from dataclasses import dataclass
import serial
import time
import random
ser = serial.Serial('/dev/ttyUSB0', baudrate=115200)


@dataclass
class Color:
    red: int = 0
    green: int = 0
    blue: int = 0

    def to_bytes(self):

        buf = b''
        buf += self.red.to_bytes(1, byteorder='big')
        buf += self.green.to_bytes(1, byteorder='big')
        buf += self.blue.to_bytes(1, byteorder='big')

        return buf

def send_color(cs_pin, led1: Color, led2: Color, action: int):
    buf = b''

    buf += cs_pin.to_bytes(1, byteorder='big')
    buf += led1.to_bytes()
    buf += led2.to_bytes()
    buf += action.to_bytes(1, byteorder='big')

    return b'###$' + len(buf).to_bytes(1, byteorder='big') + buf


while True:
    line = ser.readline()
    if line.startswith(b'###!ready'):
        print("flower setup complete")
        break
    elif line.startswith(b'###!alive'):
        print("got flower heartbeat")
        break

while True:
    rand_tile = random.randrange(0, 10)
    print(f"Setting color for tile {rand_tile}")
    col = Color(random.randrange(0, 255), random.randrange(0, 255), random.randrange(0, 255))
    data = send_color(rand_tile, col, col, 0)
    print(data)
    ser.write(data)
    time.sleep(1)

ser.close()
