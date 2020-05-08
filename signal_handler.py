# completer
import rlcompleter
import readline
readline.parse_and_bind("tab: complete")

import signal
import sys
import threading
import time

is_running = True

def example_thread():
    global is_running
    while(is_running):
        time.sleep(0.01)

def receiveSignal(signalNumber, frame):
    print('Received:', signalNumber)
    if(signalNumber == 21):
        # quit()
        sys.exit()
    return

if __name__ == "__main__":
    # register the signals to be caught
    # signal.signal(signal.SIGHUP, receiveSignal)
    signal.signal(signal.SIGINT, receiveSignal)
    # signal.signal(signal.SIGQUIT, receiveSignal)
    signal.signal(signal.SIGILL, receiveSignal)
    # signal.signal(signal.SIGTRAP, receiveSignal)
    signal.signal(signal.SIGABRT, receiveSignal)
    # signal.signal(signal.SIGBUS, receiveSignal)
    signal.signal(signal.SIGFPE, receiveSignal)
    # signal.signal(signal.SIGKILL, receiveSignal)
    # signal.signal(signal.SIGUSR1, receiveSignal)
    signal.signal(signal.SIGSEGV, receiveSignal)
    # signal.signal(signal.SIGUSR2, receiveSignal)
    # signal.signal(signal.SIGPIPE, receiveSignal)
    signal.signal(signal.SIGBREAK, receiveSignal)
    signal.signal(signal.SIGTERM, receiveSignal)
    # ex_thread = threading.Thread(target=example_thread)
    # ex_thread.start()