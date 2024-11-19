from __future__ import annotations

import struct
import random
import time
from serial import Serial
import serial.tools.list_ports
from dataclasses import dataclass


@dataclass
class Packet:
    packet_id: int
    type: int
    data: int

    FORMAT = "<L L L"
    SIZE = struct.calcsize(FORMAT)

    def pack(self) -> bytes:
        return struct.pack(Packet.FORMAT, self.packet_id, self.type, self.data)

    @staticmethod
    def unpack(data: bytes) -> Packet:
        packet_id, type, packet_data = struct.unpack(Packet.FORMAT, data)
        return Packet(packet_id=packet_id, type=type, data=packet_data)


next_id = 0


def get_id() -> int:
    global next_id
    next_id += 1
    return next_id - 1


FAST_KIND = 0
SLOW_KIND = 1


def get_random_packet() -> Packet:
    id = get_id()
    kind = random.choice([FAST_KIND, SLOW_KIND])
    data = random.randint(200, 300)
    return Packet(packet_id=id, type=kind, data=data)


def get_random_packets(max_packets: int) -> list[Packet]:
    packets = random.randint(1, max_packets)
    return [get_random_packet() for _ in range(packets)]


class Connection:
    def __init__(self, ser: Serial):
        self.ser = ser

    def send(self, data: bytes) -> None:
        assert self.ser.is_open
        self.ser.write(data)

    def recv(self, size: int) -> bytes:
        assert self.ser.is_open
        data = self.ser.read(size)
        assert len(data) == size
        return data

    def send_packet(self, packet: Packet) -> None:
        self.send(packet.pack())

    def recv_packet(self) -> Packet:
        return Packet.unpack(self.recv(Packet.SIZE))


def find_rs232() -> str:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "RS232" in port.description:
            return port.device
    assert False, "failed to detect RS232 device"


def main() -> None:
    random.seed(42)
    ser = Serial(
        port=find_rs232(),
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
    )

    with ser:
        conn = Connection(ser=ser)
        start = time.time()
        for _ in range(10):
            packets = get_random_packets(max_packets=16)
            for packet in packets:
                conn.send_packet(packet=packet)
                print(f"sent {packet}")
            for _ in packets:
                packet = conn.recv_packet()
                print(f"recv {packet}")
            # time.sleep(10 * random.random())
        end = time.time()
        print(f"time {end - start}")


if __name__ == "__main__":
    main()
