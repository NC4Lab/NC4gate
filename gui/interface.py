import os
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QWidget, QLineEdit, QLabel, QHBoxLayout,  QGroupBox, QTextEdit, QStatusBar

from PyQt5.QtCore import QTimer,  QSize
from PyQt5.QtGui import QIcon, QFont, QPixmap
from PyQt5.QtWidgets import QFileDialog
from PyQt5.uic import loadUi

from datetime import datetime
import serial
import serial.tools.list_ports

from PyQt5.QtCore import QThread, pyqtSignal

class GateControlApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.init_ui()

        self.central_widget.queryComPortButton.setEnabled(False)
        com_ports = serial.tools.list_ports.comports()
        print(list(com_ports))
        self.central_widget.comPortComboBox.addItem("Select COM Port")
        for port in com_ports:
            self.central_widget.comPortComboBox.addItem(port.device)
        self.central_widget.comPortComboBox.currentIndexChanged.connect(self.com_port_selected)
    
        self.central_widget.queryComPortButton.clicked.connect(self.query_com_port)
    
    def query_com_port(self):
        com_port = self.central_widget.comPortComboBox.currentText()
        print(f"Querying COM Port: {com_port}")

        self.arduino = serial.Serial(com_port, 115200, timeout=1)
        self.send_cmd_to_arduino("Q")

    
    def send_cmd_to_arduino(self, cmd):
        if self.arduino.isOpen():
            self.arduino.write(cmd.encode())
            print(f"Sent command to Arduino: {cmd}")
        else:
            print("Arduino serial port is not open. Please connect to Arduino first.")

    def com_port_selected(self):
        if self.central_widget.comPortComboBox.currentText() != "Select COM Port":
            self.central_widget.queryComPortButton.setEnabled(True)
        else:
            self.central_widget.queryComPortButton.setEnabled(False)

    def init_ui(self):
        self.setWindowTitle("NC4 Maze Gate Controller")
        self.setGeometry(400, 400, 800, 600)
        self.setWindowIcon(QIcon('app_icon.png'))  

        self.central_widget = QWidget(self)
        ui_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'interface.ui')
        loadUi(ui_file, self.central_widget)
        self.setCentralWidget(self.central_widget)



        # layout = QVBoxLayout(self.central_widget

        # self.load_send_csv_button = QPushButton("Initialize Maze Gate")
        # # self.load_send_csv_button.clicked.connect(self.load_and_send_csv_data)
        # layout.addWidget(self.load_send_csv_button)




        # self.setStyleSheet("""
        #     QWidget { font-size: 18px; }
        #     QPushButton { 
        #         padding: 5px; 
        #         border-radius: 10px; 
        #         border: 2px solid #4A4A4A;  /* Add border to buttons */
        #         background-color: #F0F0F0;
        #     }
        #     QPushButton:hover { background-color: #A3C1DA; }
        #     QPushButton:pressed { background-color: #629AA9; } /* Darker shade on press */
        #     QLineEdit, QTextEdit { border-radius: 5px; padding: 5px; }
        #     QGroupBox { margin-top: 20px; font-weight: bold; }
        #     QLabel { color: #555; }
        # """)

    #     mouseInfoLayout = QHBoxLayout()
    #     self.mouse_name_input = QLineEdit()
    #     self.mouse_name_input.setPlaceholderText("Enter Mouse ID here")
    #     self.save_mouse_name_button = QPushButton("Save")
    #     self.save_mouse_name_button.setIcon(QIcon('save_icon.png')) 
    #     self.save_mouse_name_button.clicked.connect(self.save_mouse_name)
    #     self.mouse_name_label = QLabel("Current Mouse Name: No Mouse Name Set")
    #     mouseInfoLayout.addWidget(self.mouse_name_input)
    #     mouseInfoLayout.addWidget(self.save_mouse_name_button)
    #     mouseInfoLayout.addWidget(self.mouse_name_label)
        
    
    # # Controls Section
    #     controlsLayout = QVBoxLayout()
        
    #     self.export_excel_button = QPushButton("Export to Excel")
    #     self.export_excel_button.clicked.connect(self.export_to_excel)
    #     controlsLayout.addWidget(self.export_excel_button)
    #     self.create_button("Prime Feeding Tube", 'tube_icon.png', self.prime_feeding_tube, controlsLayout)
    #     self.create_button("Stop Priming", 'stop_icon.png', self.stop_priming, controlsLayout)
    #     self.create_button("Start Training", 'start_icon.png', self.start_training, controlsLayout)
    #     self.create_button("Stop Training", 'stop_icon.png', self.stop_training, controlsLayout)
    #     self.create_button("Buzzer", 'buzzer_icon.png', self.activate_buzzer, controlsLayout)

    #     controlsGroup = QGroupBox("Controls")
    #     controlsGroup.setLayout(controlsLayout)
    #     layout.addLayout(mouseInfoLayout)
    #     layout.addWidget(controlsGroup)

    # def create_button(self, text, icon_path, callback, layout):
    #     btn = QPushButton(text)
    #     btn.setIcon(QIcon(icon_path))
    #     btn.clicked.connect(callback)
    #     layout.addWidget(btn)
        


    # def init_serial_connection(self):
    #     try:
    #         self.arduino = serial.Serial('COM12', 115200, timeout=1)
    #         self.serial_timer = QTimer()
    #         self.serial_timer.timeout.connect(self.read_from_serial)
    #         self.serial_timer.start(100)
    #     except serial.SerialException as e:
    #         print(f"Failed to connect to Arduino: {e}")
    # def export_to_excel(self):

    #     options = QFileDialog.Options()
    #     fileName, _ = QFileDialog.getSaveFileName(self, "Save Session", "", "Excel Files (*.xlsx)", options=options)
    #     if fileName:
    #         if not fileName.endswith('.xlsx'):
    #             fileName += '.xlsx'
    #         self.save_session_to_excel(fileName)
            
           
    
    # def load_and_send_csv_data(self):
    #     # Get the CSV file path from the user
    #     options = QFileDialog.Options()
    #     fileName, _ = QFileDialog.getOpenFileName(self, "Open CSV File", "", "CSV Files (*.csv)", options=options)
    
    #     if fileName:
    #         df = pd.read_csv(fileName, header=None)
        
    #         # Format the CSV data to 16-character strings per trial
    #         formatted_data = df.apply(
    #             lambda x: '{:<4}{:<4}{:<4}{:<1}'.format(
    #                 x[0] if pd.notnull(x[0]) else '',
    #                 x[1] if pd.notnull(x[1]) else '',
    #                 x[2] if pd.notnull(x[2]) else '',
    #                 x[3] if pd.notnull(x[3]) else ''
    #             ),
    #             axis=1
    #         )

    #         # Join all trials into one string with a space separator after every 16 characters
    #         data_string = ' '.join(formatted_data) + '\n'  # Add newline to indicate the end

    #         # Debug print the actual data being sent (spaces will show as explicit \s)
    #         print(repr(data_string))

    #         # Sending the data to ESP32
    #         self.send_to_esp32(data_string)
        


    
    # def send_to_esp32(self, data_string):
    #     if self.arduino.isOpen():
    #         self.arduino.write(data_string.encode())
    #         # After sending the last row, listen for acknowledgment
    #         if data_string.endswith('\n'):
    #             print("Waiting for ESP32 acknowledgment...")
    #             while True:
    #                 if self.arduino.inWaiting() > 0:
    #                     response = self.arduino.readline().decode('utf-8').strip()
    #                     print("Received from ESP32:", response)
    #                     if response == "CSV Data Processed":
    #                         print("ESP32 has processed the CSV data.")
    #                         break
 


    # def prime_feeding_tube(self):
    #     self.arduino.write(b'p')
    #     print("Priming feeding tube initiated.")

    # def stop_priming(self):
    #     self.arduino.write(b'x')
    #     print("Priming stopped.")

    # def start_training(self):
    #     self.current_trial_number = 1 
    #     current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
    #     self.start_new_trial()  
    #     self.current_trial["Training Started"] = current_time
    #     self.arduino.write(b's')
    #     print(f"{current_time} - TRAINING STARTED!!!")

    # def stop_training(self, auto_stopped=False):
    #     current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
    #     if self.current_trial is not None: 
    #         self.current_trial["Training Stopped"] = current_time
    #         self.trial_data.append(self.current_trial)
    #         self.current_trial = None  
    #         self.arduino.write(b't')
    #         print(f"{current_time} - TRAINING STOPPED!!!")

    
    # def activate_buzzer(self):
    #     self.arduino.write(b'1')
    #     print("Buzzer activated manually")

    # def read_from_serial(self):
    #     if self.arduino.isOpen() and self.arduino.inWaiting() > 0:
    #         serial_line = self.arduino.readline().decode('utf-8', errors='ignore').strip()
    #         self.parse_and_record_data(serial_line)
            
    # def save_mouse_name(self):
    #     self.mouse_name = self.mouse_name_input.text().strip()

    #     if self.mouse_name:
    #         # Update the label with the new mouse name
    #         self.mouse_name_label.setText(f"Current Mouse Name: {self.mouse_name}")
    #         print(f"Mouse name set to '{self.mouse_name}'")  
    #         self.mouse_name_input.clear()
    #     else:
    #         print("Mouse name cannot be empty. Please enter a valid mouse name.")



    # def parse_and_record_data(self, serial_line):
    #     current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
    #     print(f"{current_time} - {serial_line}")  
        
    #     if self.current_trial is None:
    #         self.start_new_trial()
            
        
    #     if serial_line == "Training Finished!!!!.":
            
           
    #          if self.current_trial is not None: 
    #             self.current_trial["Training Finished"] = current_time
    #             self.trial_data.append(self.current_trial)
    #             self.current_trial = None
    #             self.update_csv()  
            
            
        
    #     # Start a new trial if a command to M0 1 is sent and the last trial ended
    #     if "Show command sent to M0 2" in serial_line:
    #         self.current_trial["Command to M0 2"] = current_time
    #     elif "Show command sent to M0 4" in serial_line:
    #         self.current_trial["Command to M0 4"] = current_time
        
    #     if self.current_trial is None:
    #         self.start_new_trial()

        
    #     elif "Respond from M0" in serial_line:
    #         # Split the string by ':' and then  split the second part by spaces.
    #         parts = serial_line.split(":")
    #         m0_id = parts[0].split()[-1] 
    #         image_id = parts[1].strip().split()[-1]  # image ID 
    #         response_key = f"Response from M0 {m0_id}"
    #         self.current_trial[response_key] = f"Image {image_id} at {current_time}"
            
            
            


    #     elif "Touch detected at:" in serial_line:
    #         self.current_trial["Touch Detected"] = True
    #         self.current_trial["Touch Location"] = serial_line.split(":")[1].strip()
    #         self.current_trial["Touch Timestamp"] = current_time
            
    #     elif serial_line in ["L", "R"]:
    #         self.current_trial["Touch Side"] = "Left" if serial_line == "L" else "Right"
             
    #     elif "Black command sent to" in serial_line:
    #          m0_id = serial_line.split()[-1][-1]  # M0 ID
    #          self.current_trial[f"Black Command to M0 {m0_id}"] = current_time
            

    #     # Save the current trial data and start a new trial after black response from M0 2
    #     if "Black Response from M0 2" in serial_line:
    #         self.current_trial["Black Response from M0 2"] = current_time
    #         self.trial_data.append(self.current_trial)
    #         self.current_trial = None  
            
    #     self.update_csv()     

    # def start_new_trial(self):
    #     if self.current_trial is not None:
    #         self.trial_data.append(self.current_trial)


    #     self.current_trial = {
    #         "Mouse Name": self.mouse_name,  
    #         "Trial Number": self.current_trial_number,
    #         "Training Started": None,  
    #         "Training Finished": None, 
    #         "Training Stopped": None,
    #         "Command to M0 1": None,
    #         "Response from M0 1": None,
    #         "Command to M0 2": None,
    #         "Response from M0 2": None,
    #         "Touch Detected": False,
    #         "Touch Location": None,
    #         "Touch Side": None,
    #         "Touch Timestamp": None,
    #         "Black Command to M0 1": None,
    #         "Black Command to M0 2": None
    #         }
    #     self.current_trial_number += 1 


    # def update_csv(self):
    #     df = pd.DataFrame(self.trial_data)
    #     df.to_csv("training_data.csv", index=False)
    #     self.statusBar().showMessage("Session exported to CSV successfully.", 5000)
        
    # def save_session_to_excel(self, file_path):
    #     df = pd.DataFrame(self.trial_data)
    #     with pd.ExcelWriter(file_path, engine='openpyxl') as writer:
    #         df.to_excel(writer, sheet_name='Trial Data', index=False)
    #     self.statusBar().showMessage("Session exported to Excel successfully.", 5000)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = GateControlApp()
    window.show()
    sys.exit(app.exec_())

