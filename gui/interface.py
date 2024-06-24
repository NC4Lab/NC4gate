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

        # Disable the COM port query button initially
        self.central_widget.queryComPortButton.setEnabled(False)
        
        # List all available COM ports
        com_ports = serial.tools.list_ports.comports()
        print(list(com_ports))  # Print available COM ports for debugging
        
        # Populate the COM port combo box with available ports
        self.central_widget.comPortComboBox.addItem("Select COM Port")
        for port in com_ports:
            self.central_widget.comPortComboBox.addItem(port.device)
        
        # Connect the combo box selection change event to the handler
        self.central_widget.comPortComboBox.currentIndexChanged.connect(self.com_port_selected)
        
        # Connect the query button click event to the handler
        self.central_widget.queryComPortButton.clicked.connect(self.query_com_port)

    # Method to handle querying the selected COM port
    def query_com_port(self):
        com_port = self.central_widget.comPortComboBox.currentText()
        print(f"Querying COM Port: {com_port}")  # Print the selected COM port for debugging

        # Open a serial connection to the Arduino on the selected COM port
        self.arduino = serial.Serial(com_port, 115200, timeout=1)
        self.send_cmd_to_arduino("Q")  # Send a query command to the Arduino

    # Method to send a command to the Arduino
    def send_cmd_to_arduino(self, cmd):
        if self.arduino.isOpen():
            self.arduino.write(cmd.encode())  # Encode and send the command
            print(f"Sent command to Arduino: {cmd}")  # Print the sent command for debugging
        else:
            print("Arduino serial port is not open. Please connect to Arduino first.")

    # Method to handle COM port selection from the combo box
    def com_port_selected(self):
        if self.central_widget.comPortComboBox.currentText() != "Select COM Port":
            self.central_widget.queryComPortButton.setEnabled(True)  # Enable the query button if a valid port is selected
        else:
            self.central_widget.queryComPortButton.setEnabled(False)  # Disable the query button if no valid port is selected

    # Method to initialize the UI
    def init_ui(self):
        self.setWindowTitle("NC4 Maze Gate Controller")  # Set the window title
        self.setGeometry(400, 400, 800, 600)  # Set the window size and position
        self.setWindowIcon(QIcon('app_icon.png'))  # Set the window icon

        # Load the UI from a .ui file
        self.central_widget = QWidget(self)
        ui_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'interface.ui')
        loadUi(ui_file, self.central_widget)
        self.setCentralWidget(self.central_widget)  # Set the central widget

# Main entry point of the application
if __name__ == "__main__":
    app = QApplication(sys.argv)  # Create a QApplication instance
    window = GateControlApp()  # Create an instance of the main window
    window.show()  # Show the main window
    sys.exit(app.exec_())  # Execute the application event loop


