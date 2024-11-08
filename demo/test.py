import serial
import serial.tools.list_ports


def find_rs232() -> str:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "RS232" in port.description:
            return port.device
    assert False, "failed to detect RS232 device"


ser = serial.Serial(
    port=find_rs232(),
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
)

with ser:
    while True:
        print(ser.readline())
