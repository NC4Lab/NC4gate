import serial
import time

# Replace 'COM7' with the correct port for your setup
ser = serial.Serial('COM7', 115200, timeout=1)

time.sleep(2)  # Wait for the serial connection to initialize
ser.reset_input_buffer()  # Clear any initial garbage data

ser.write(b'Hello Arduino\n')  # Send a test message

while True:
    if ser.in_waiting > 0:
        line = ser.readline()
        try:
            decoded_line = line.decode('utf-8').rstrip()
            print(f"Received (decoded): {decoded_line}")
        except UnicodeDecodeError:
            print(f"Received (raw): {line}")

# import serial
# import time

# # Replace 'COM7' with the correct port for your setup
# ser = serial.Serial('COM7', 115200, timeout=1)

# time.sleep(2)  # Wait for the serial connection to initialize

# ser.write(b'Hello Arduino\n')  # Send a test message

# while True:
#     if ser.in_waiting > 0:
#         line = ser.readline()
#         try:
#             decoded_line = line.decode('utf-8').rstrip()
#             print(f"Received (decoded): {decoded_line}")
#         except UnicodeDecodeError:
#             print(f"Received (raw): {line}")
