from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit
from PySide6.QtGui import QIcon, QPalette, QColor, QFont
from PySide6.QtCore import Qt, QSize
import pyqtgraph as pg
from PySide6.QtGui import QPixmap
from PySide6.QtCore import QTimer
from bitstring import BitArray
import serial

#----- PROTOCOL -----
RIGHT_LED_ROTATION = '1'
LEFT_LED_ROTATION = '2'
STOP_LEDS='0'
#--------------------

class MainWindow(QMainWindow):
    promotie: str = "2023-2024"
    team: list[str] = [
        "CORBU IONUT-DANIEL",
        "VOICA IOANA",
    ]
    def __init__(self):
        super().__init__()
        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))
        self.ser = serial.Serial(port='COM3', baudrate=14400)

        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member,1)
        team_box_layout.addWidget(second_member,1)
        team_box.setLayout(team_box_layout)

        control_panel_box = QGroupBox("LEDS ROTATION")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("WHITE-GREEN-PINK-BLACK")
        button1.clicked.connect(self.button1_clicked)
        button2 = QPushButton("BLACK-PINK-GREEN-WHITE")
        button2.clicked.connect(self.button2_clicked)
        button3 = QPushButton("Stop Leds")
        button3.clicked.connect(self.button3_clicked)
       
        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(button1,1)
        control_panel_box_layout.addWidget(button2,1)
        control_panel_box_layout.addWidget(button3,1)
        control_panel_box_layout.addStretch()
     

        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box,5)
        
        self.values = []
        
        categories = ['Green', 'Yellow', 'Red']
        self.bar_plot_widget = pg.PlotWidget()
        self.bar_plot_widget.setBackground('w')
        self.bar_plot = pg.BarGraphItem(x=[1, 2, 3], height=[0, 0, 0], width=0.2, brushes=['g', 'y', 'r'])
        self.bar_plot_widget.addItem(self.bar_plot)

        secondary_layout.addWidget(self.bar_plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 4)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        widget = QWidget()
        widget.setLayout(primary_layout)
        
        self.setCentralWidget(widget)
        
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.read_serial_data)
        self.timer.start(100)

    def send_input(self):
        input = self.line_edit.text()
        self.line_edit.clear()
        self.text_edit.insertPlainText(f"INPUT: {input}\n")
        
    def button1_clicked(self):
        self.ser.write(bytearray(RIGHT_LED_ROTATION,'ascii'))
        
    def button2_clicked(self):
        self.ser.write(bytearray(LEFT_LED_ROTATION,'ascii'))
    
    def button3_clicked(self):
        self.ser.write(bytearray(STOP_LEDS,'ascii'))
        
    def update_bar_plot(self):
        categories = ['Green', 'Yellow', 'Red']

        # Intervalul de valori este divizat dinamic
        min_value = min(self.values)
        max_value = max(self.values)
        value_range = max_value - min_value
        if value_range == 0:
            value_range = 1  # Evit impartirea prin zero

        # Definesc dinamic pragurile pentru impartirea culorilor 
        green_threshold = min_value + 0.33 * value_range
        yellow_threshold = min_value + 0.66 * value_range

        # Clasificarea pe culori:
        # verde - valorile mici
        # galben - valorile medii
        # rosu - valorile mari
        colors = ['g' if val < green_threshold else 'y' if val < yellow_threshold else 'r' for val in self.values]

        # Update bar plot 
        self.bar_plot.setOpts(x=[1, 2, 3], height=[colors.count('g'), colors.count('y'), colors.count('r')])

    def read_serial_data(self):
        data = self.ser.read(4)
        
        if(len(data) == 4):
            
            # Converteste data in binary
            binary_data = ''.join(format(byte, '08b') for byte in data)

            # Converteste binary data in BitArray
            bit_array = BitArray(bin=binary_data)

            # Inversez ordinea bitilor primiti (convert MSB to LSB)
            reversed_bit_array = bit_array[::-1]

            # Transform  BitArray inversat  inapoi in bytes
            reversed_data = reversed_bit_array.bytes
            value = reversed_data.decode('ascii')
            value = float(value[::-1])
            print(value)
            
            self.values.append(value)
        self.update_bar_plot()
            
        
    