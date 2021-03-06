# completer
import rlcompleter
import readline
readline.parse_and_bind("tab: complete")

import serial
import time
import signal
import sys
import threading

# import PySimpleGUI as sg

# sg.theme('DarkAmber')	# Add a touch of color
# # All the stuff inside your window.
# layout = [  [sg.Text('Some text on Row 1')],
#             [sg.Text('Enter something on Row 2'), sg.InputText()],
#             [sg.Button('Ok'), sg.Button('Cancel')] ]

is_running = False

def get_serial_port():
    import serial.tools.list_ports   # import serial module
    comPorts = list(serial.tools.list_ports.comports())    # get list of all devices connected through serial port
    if len(comPorts)==0:
        print 'No COM port found!'
        return
    else:
        for i in range(len(comPorts)):
            print i+1,'.',comPorts[i] 
        choose = raw_input('Choose COM port : ')
        serial_port_string = str(comPorts[int(choose)-1])
        serial_port=''
        for i in serial_port_string:
            if(i==' '):
                break
            serial_port+=i
    return serial_port

def read_thread():
    global is_running
    while (is_running):
        rx_data = ser.read_until('\n')
        if(len(rx_data)>0):
            print rx_data

def signal_handler(signalNumber, frame):
    global is_running
    is_running = False
    print 'read thread stop'

if __name__ == '__main__':
    # serial_port = get_serial_port()
    ser=serial.Serial(get_serial_port(),38400,timeout=0.5) # same as bluetooth baudrate
    signal.signal(signal.SIGINT, signal_handler)
    is_running = True
    rx_thread = threading.Thread(target=read_thread)
    rx_thread.start()

def get_app_name():
    ser.write('0,\n')

def save_config():
    ser.write('1,\n')

def reset_firmware():
    ser.write('2,\n')

def set_aux_in_params(id,scale,offset):
    send_data = '{0},{1},{2},{3},\n'.format(3,id,scale,offset)
    ser.write(send_data)

def get_aux_in_params(id):
    send_data = '{0},{1},\n'.format(4,id)
    ser.write(send_data)

def read_aux_in(id):
    send_data = '{0},{1},\n'.format(5,id)
    ser.write(send_data)

def read_frequency():
    send_data = '{0},\n'.format(6)
    ser.write(send_data)
    
def set_brightness(value):
    send_data = '{0},{1},\n'.format(7,value)
    ser.write(send_data)

def set_text(text):
    send_data = '{0},{1},\n'.format(8,text)
    ser.write(send_data)

def set_alarm_freq_threshold(upper,lower):
    send_data = '{0},{1},{2},\n'.format(9,upper,lower)
    ser.write(send_data)

def get_alarm_freq_threshold():
    send_data = '{0},\n'.format(10)
    ser.write(send_data)