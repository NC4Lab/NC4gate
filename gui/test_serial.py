import serial
import time

# Replace 'COM3' with the correct port for your setup
ser = serial.Serial('COM10', 115200, timeout=1)

time.sleep(2)  # Wait for the serial connection to initialize

ser.write(b'Hello Arduino\n')  # Send a test message

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()
        print(f"Received: {line}")