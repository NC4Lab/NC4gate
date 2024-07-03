# Import necessary modules
import os
import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QPushButton, QVBoxLayout, QWidget,
                             QLineEdit, QLabel, QHBoxLayout, QGroupBox, QTextEdit, QStatusBar)
from PyQt5.QtCore import QTimer, QSize
from PyQt5.QtGui import QIcon, QFont, QPixmap
from PyQt5.QtWidgets import QFileDialog
from PyQt5.uic import loadUi
from datetime import datetime
import serial
import serial.tools.list_ports
from PyQt5.QtCore import QThread, pyqtSignal

# Define the main application class inheriting from QMainWindow


class GateControlApp(QMainWindow):
    def __init__(self):
        super().__init__()  # Initialize the parent class
        self.init_ui()  # Call the UI initialization method

        # Disable the init buttons initially
        self.ui_widget.init_system_button.setEnabled(False)
        self.ui_widget.init_gates_button.setEnabled(False)

        # List all available COM ports
        com_ports = serial.tools.list_ports.comports()
        # print(list(com_ports))  # Print available COM ports for debugging

        # Populate the COM port combo box with available ports
        self.ui_widget.com_port_combo_box.addItem("Select COM Port")
        for port in com_ports:
            self.ui_widget.com_port_combo_box.addItem(port.device)

        # Connect the combo box selection change event to the handler
        self.ui_widget.com_port_combo_box.currentIndexChanged.connect(
            self.com_port_selected)

        # Connect the cypress initialization button click event to the handler
        self.ui_widget.init_system_button.clicked.connect(
            self.callback_init_system_button)

        # Connect the gates initialization button click event to the handler
        self.ui_widget.init_gates_button.clicked.connect(
            self.callback_init_gates_button)

        # Connect the send gate configuration button click event to the handler
        self.ui_widget.send_gate_config_button.clicked.connect(
            self.callback_send_gate_config_button)

        # Setup arduino serial communication stuff
        self.arduino = None
        self.timer_check_serial = QTimer(self)
        self.timer_check_serial.timeout.connect(self.check_receive_message)
        self.pending_message = None

        # Define start and end bytes
        self.START_BYTE = b'\x02'  # Start byte
        self.END_BYTE = b'\x03'    # End byte

        # Define message check delay
        self.MESSAGE_CHECK_DT = 50  # ms

        # Resonse timeout parameters
        self.RESPONSE_TIMEOUT = 30000  # Resonse timeout (ms)

        # Timeout timer
        self.timeout_timer = QTimer(self)
        # Ensure the timer only fires once
        self.timeout_timer.setSingleShot(True)
        self.timeout_timer.timeout.connect(self.handle_timeout)

        # Initialize a dictionary for message data
        # Fields:
        #   'msg_type': Integer indicating the type of message.
        #       0: Initialize cypress
        #       1: Initialize gates
        #       2: Move gates
        #   'data': List of integers representing the data associated with the message.
        #       Initialized with a list of 100 zeros.
        #   'length': Integer indicating the length of the data.
        #       Initialized to 0.
        self.message_data = {
            'msg_type': 0,            # Initialize with default values
            'data': [0] * 100,        # Initialize with a list of 100 zeros
            'length': 0
        }

        # Initialize a list for storing cypress configuration data
        # This list will contain dictionaries, each representing an entry with:
        #   'i2c_addr': Hex of the I2C address associated with the entry.
        #   'enabled_gates': List of integers for the enabled gates for the entry.
        #   'active_gates': List of integers for the active (up) gates for the entry.
        self.cypress_list = []

    # Method to initialize the UI
    def init_ui(self):
        # Set the window title
        self.setWindowTitle("NC4 Maze Gate Controller")

        # Load the UI from a .ui file
        self.ui_widget = QWidget(self)
        ui_file = os.path.join(os.path.dirname(
            os.path.realpath(__file__)), 'interface.ui')
        loadUi(ui_file, self.ui_widget)
        self.setCentralWidget(self.ui_widget)  # Set the central widget

        # Move window to upper left and set the fixed size based on the dimensions from the UI file
        main_window_width = self.ui_widget.geometry().width()
        main_window_height = self.ui_widget.geometry().height()
        self.setGeometry(200, 200, main_window_width, main_window_height)

        # Set the window icon
        self.setWindowIcon(QIcon('app_icon.png'))  # Set the window icon

        # Initialize the structure to store QGroupBox, QLabel, and QPushButton objects
        self.widget_groups = {}
        for group_index in range(9):
            # Get objects for this index
            group_box = self.findChild(QGroupBox, f'cypress_box_{group_index}')
            label = self.findChild(QLabel, f'i2c_label_{group_index}')
            buttons = [self.findChild(QPushButton, f'gate_button_{group_index}_{button_index}')
                       for button_index in range(8)]

            # Store the objects in the dictionary
            self.widget_groups[group_index] = {
                'group_box': group_box,
                'label': label,
                'buttons': buttons
            }

            # Disable the group box
            group_box.setEnabled(False)
            group_box.setStyleSheet("QGroupBox::title { color: gray; }")

            # Disable all buttons in the group
            for button in buttons:
                button.setEnabled(False)

    # Method to handle COM port selection from the combo box
    def com_port_selected(self):
        if self.ui_widget.com_port_combo_box.currentText() != "Select COM Port":
            # Enable the query button if a valid port is selected
            self.ui_widget.init_system_button.setEnabled(True)
        else:
            # Disable the query button if no valid port is selected
            self.ui_widget.init_system_button.setEnabled(False)

    # Method to handle cypress initialization button press
    def callback_init_system_button(self):

        # Get selected com port
        com_port = self.ui_widget.com_port_combo_box.currentText()

        # Open a serial connection to the Arduino on the selected COM port
        self.arduino = serial.Serial(com_port, 115200, timeout=1, dsrdtr=True)

        # Send the command to Arduino
        self.send_message(0, 0)

        print(f"Initializing System Through COM Port: {com_port}")

    # Method to handle gates initialization button press
    def callback_init_gates_button(self):
        # Send init gates message
        self.send_message(1, 0)

        print(f"Initializing Gates")

    # Method to handle send gate configuration button press
    def callback_send_gate_config_button(self):
        active_gates_byte_array = [0] * len(self.cypress_list)

        # Loop through the cypress list
        for i in range(len(self.cypress_list)):
            # Get the gates array
            gate_array = self.cypress_list[i]['enabled_gates']
            active_gates_array = []

            # Loop through the gates and check if the respective button is active
            for gate in gate_array:
                # Check if the button is pressed
                if self.widget_groups[i]['buttons'][gate].isChecked():
                    active_gates_array.append(gate)

            # Update the gates array with active gates
            self.cypress_list[i]['active_gates'] = active_gates_array

            # Convert array index to a byte and store it in the byte array
            active_gates_byte_array[i] = self.ind_array_2_byte(
                active_gates_array)

            # Print the active gates for debugging
            print(
                f"Send move command for Cypress {i} Gates {active_gates_array}")

        # Send the gate configuration message
        self.send_message(2, active_gates_byte_array)

    # Method to send a message to the Arduino via serial
    def send_message(self, msg_type, data):

        if self.arduino and self.arduino.isOpen():
            # Convert the message type to a single byte
            msg_type_byte = msg_type.to_bytes(1, 'little')

            # Ensure data is a list or array of integers
            if isinstance(data, int):
                data = [data]

            # Convert the data array to bytes
            data_bytes = bytes(data)

            # Convert the length to a single byte
            length_byte = len(data_bytes).to_bytes(1, 'little')

            # Calculate the checksum
            checksum = (sum(data_bytes)) % 256
            checksum_byte = checksum.to_bytes(1, 'little')

            # Construct the message with start byte, message type, length, data, checksum, and end byte
            message = self.START_BYTE + msg_type_byte + length_byte + \
                data_bytes + checksum_byte + self.END_BYTE
            self.arduino.write(message)  # Send the message as bytes
            self.pending_message = data  # Store the pending message

            # Start the timer to check for response
            self.timer_check_serial.start(self.MESSAGE_CHECK_DT)

            # Start the timeout timer
            self.timeout_timer.start(self.RESPONSE_TIMEOUT)

            # Uncomment to print the sent message
            # print(f"Sent command to Arduino:")
            # print(f"  Start Byte: {self.START_BYTE}")
            # print(f"  Message Type Byte: {
            #       int.from_bytes(msg_type_byte, 'little')}")
            # print(f"  Length Byte: {int.from_bytes(length_byte, 'little')}")
            # print(f"  Data Bytes: {[byte for byte in data_bytes]}")
            # print(f"  Checksum Byte: {
            #       int.from_bytes(checksum_byte, 'little')}")
            # print(f"  End Byte: {self.END_BYTE}")
            # print(f"  Full Message: {[byte for byte in message]}")
        else:
            print("Arduino serial port is not open. Please connect to Arduino first.")

    # Method to check for a response from the Arduino
    def check_receive_message(self):
        if self.arduino and self.arduino.in_waiting:
            message = self.arduino.read_all()  # Read the response as bytes

            # Check for start and end bytes
            if message and message[0:1] == self.START_BYTE and message[-1:] == self.END_BYTE:
                # Extract the message type byte and store it in the dictionary
                self.message_data['msg_type'] = int.from_bytes(
                    message[1:2], 'little')

                # Extract the length byte and store it in the dictionary
                self.message_data['length'] = int.from_bytes(
                    message[2:3], 'little')

                # Extract the data bytes and store them in the dictionary
                self.message_data['data'] = list(
                    message[3:3+self.message_data['length']])

                # Extract the checksum byte
                checksum_byte = message[3+self.message_data['length']]

                # Verify checksum
                checksum_calculated = (
                    sum(self.message_data['data']) + self.message_data['msg_type']) % 256
                if checksum_byte == checksum_calculated:

                    # Update variables
                    self.timer_check_serial.stop()  # Stop the timer if a valid response is received
                    self.timeout_timer.stop()  # Stop the timeout timer
                    self.pending_message = None  # Clear the pending message

                    # Process the received message
                    self.proc_receive_message()

                    # Uncomment to print the received message
                    print(f"Received response from Arduino:")
                    print(f"  Start Byte: {self.START_BYTE}")
                    print(f"  Message Type Byte: {
                          self.message_data['msg_type']}")
                    print(f"  Length Byte: {self.message_data['length']}")
                    print(f"  Data Bytes: {[hex(byte)
                          for byte in self.message_data['data']]}")
                    print(f"  Checksum Byte: {checksum_byte}")
                    print(f"  End Byte: {self.END_BYTE}")
                    print(f"  Full Message: {[byte for byte in message]}")
                else:
                    print(f"Invalid checksum. Expected: {
                          checksum_byte}, Calculated: {checksum_calculated}")
            else:
                print(f"Invalid response format: {message}")

    # Method to handle timeout

    def handle_timeout(self):
        error_msg = f"ERROR: No response for command: {
            self.pending_message} within {self.RESPONSE_TIMEOUT} ms"
        print(error_msg)  # Print the error message
        self.ui_widget.error_label.setText(
            error_msg)  # Display the error message
        self.timer_check_serial.stop()  # Stop checking for messages
        self.pending_message = None  # Clear the pending message

    # Method to process the received message
    def proc_receive_message(self):

        # Process initialize system message
        if self.message_data['msg_type'] == 0:

            # Loop through the received data
            for i in range(self.message_data['length']):
                # Store the active I2C addresses
                entry = {
                    # Set i2c_addr to the value from data
                    'i2c_addr': self.message_data['data'][i],
                    'enabled_gates': [],  # Initialize entry
                    'active_gates': []  # Initialize entry
                }
                self.cypress_list.append(entry)

                # Enable the group boxes
                self.widget_groups[i]['group_box'].setEnabled(True)
                self.widget_groups[i]['group_box'].setStyleSheet(
                    "QGroupBox::title { color: green; }")

                # Change the I2C address label
                self.widget_groups[i]['label'].setText(hex(entry['i2c_addr']))

            # Disable the init system button
            self.ui_widget.init_system_button.setEnabled(False)

            # Enable init gates button
            self.ui_widget.init_gates_button.setEnabled(True)

        # Process initialize gates message
        elif self.message_data['msg_type'] == 1:

            # Loop through the received data
            for i in range(self.message_data['length']):
                # Store the enabled gates for each I2C addresses
                gate_state = self.byte_2_ind_array(
                    self.message_data['data'][i])
                self.cypress_list[i]['enabled_gates'] = gate_state

                # Change button features
                for gate in self.cypress_list[i]['enabled_gates']:
                    self.widget_groups[i]['buttons'][gate].setEnabled(
                        True)  # Enable the buttons
                    self.widget_groups[i]['buttons'][gate].setStyleSheet(
                        "background-color: lightgreen;")  # Set the button color to light green

            # Disable the init gates button
            self.ui_widget.init_gates_button.setEnabled(False)

        # Process move gates message
        elif self.message_data['msg_type'] == 2:

            # Loop through the received data
            for i in range(self.message_data['length']):
                # Get the returned gate states
                gate_state = set(self.byte_2_ind_array(
                    self.message_data['data'][i]))

                # Get a list of gates that should be active
                active_gates = set(self.cypress_list[i]['active_gates'])

                # Set all buttons to light green
                for gate in active_gates:
                    self.widget_groups[i]['buttons'][gate].setStyleSheet(
                        "background-color: lightgreen;")

                # Find mismatched gates
                mismatched_gates = list(
                    (active_gates - gate_state) | (gate_state - active_gates))

                # Flag and print mismatched gates
                for gate in mismatched_gates:
                    # Print the error message
                    # Print the error message
                    print(f"ERROR: Gate {gate} for Cypress {i} was not moved.")
                    self.ui_widget.error_label.setText(
                        "ERROR: Gate move failed")  # Display the error message
                    # Make the button red
                    self.widget_groups[i]['buttons'][gate].setStyleSheet(
                        "background-color: red;")
                    # Reset the button to it's previous state
                    if self.widget_groups[i]['buttons'][gate].isChecked():
                        self.widget_groups[i]['buttons'][gate].setChecked(
                            False)
                    else:
                        self.widget_groups[i]['buttons'][gate].setChecked(True)

                # Clear the error if no mismatched gates
                if not mismatched_gates:
                    self.ui_widget.error_label.setText("")

    # Method to convert a byte to an array of indices corresponding to bits set to 1
    def byte_2_ind_array(self, byte):
        return [i for i in range(8) if byte & (1 << i)]

    # Method to convert an array to a single byte with bits set to 1 for each index
    def ind_array_2_byte(self, array):
        byte = 0
        for wall_i in array:
            if 0 <= wall_i <= 7:
                byte |= (1 << wall_i)
        return byte


# Main entry point of the application
if __name__ == "__main__":
    app = QApplication(sys.argv)  # Create a QApplication instance
    window = GateControlApp()  # Create an instance of the main window
    window.show()  # Show the main window
    sys.exit(app.exec_())  # Execute the application event loop
