# completer
import rlcompleter
import readline
readline.parse_and_bind("tab: complete")

import serial
import time
import signal
import sys
import threading

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

if __name__ == '__main__':
    serial_port = get_serial_port()
    ser=serial.Serial(serial_port,9600)
