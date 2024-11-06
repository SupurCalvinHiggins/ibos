from __future__ import annotations

import struct
import random
import time
from serial import Serial
from dataclasses import dataclass


def compute_checksum(data: bytes) -> int:
    return 0


@dataclass(frozen=True)
class Header:
    version: int
    content_kind: int
    content_size: int

    FORMAT = "B B >H"
    SIZE = struct.calcsize(FORMAT)

    def pack(self) -> bytes:
        data = struct.pack(
            Header.FORMAT, self.version, self.content_kind, self.content_size
        )
        assert len(data) == Header.SIZE
        return data

    @staticmethod
    def unpack(data: bytes) -> Header:
        assert len(data) == Header.SIZE
        version, content_kind, content_size = struct.unpack(Header.FORMAT, data)
        return Header(
            version=version, content_kind=content_kind, content_size=content_size
        )


@dataclass(frozen=True)
class Content:
    value: bytes

    def pack(self) -> bytes:
        return self.value

    @staticmethod
    def unpack(data: bytes) -> Content:
        value = data
        return Content(value=value)


@dataclass(frozen=True)
class Checksum:
    value: int

    FORMAT = ">H"
    SIZE = struct.calcsize(FORMAT)

    def pack(self) -> bytes:
        data = struct.pack(Checksum.FORMAT, self.value)
        assert len(data) == Checksum.SIZE
        return data

    @staticmethod
    def unpack(data: bytes) -> Checksum:
        assert len(data) == Checksum.SIZE
        value = struct.unpack(Checksum.FORMAT, data)[0]
        return Checksum(value=value)


@dataclass
class Packet:
    header: Header
    content: Content
    checksum: Checksum

    def pack(self) -> bytes:
        data = bytearray()
        data.extend(self.header.pack())
        data.extend(self.content.pack())
        data.extend(self.checksum.pack())
        return bytes(data)

    @staticmethod
    def unpack(data: bytes) -> Packet:
        header_start, header_end = 0, Header.SIZE
        header = Header.unpack(data[header_start:header_end])
        content_start, content_end = header_end, header_end + header.content_size
        content = Content.unpack(data[content_start:content_end])
        checksum_start, checksum_end = content_end, content_end + Checksum.SIZE
        checksum = Checksum.unpack(data[checksum_start:checksum_end])
        return Packet(header=header, content=content, checksum=checksum)


TSP_REQ_KIND = 0
MAT_REQ_KIND = 1
TSP_RSP_KIND = 0
MAT_RSP_KIND = 1

MAX_CONTENT_SIZE = 512 - Header.SIZE - Checksum.SIZE


def get_random_req_packet() -> Packet:
    version = 0
    content_kind = random.choice([TSP_REQ_KIND, MAT_REQ_KIND])
    content_size = random.choice([i**2 for i in range(int(MAX_CONTENT_SIZE**0.5))])
    header = Header(
        version=version, content_kind=content_kind, content_size=content_size
    )

    content_value = bytes(random.randint(0, 255) for _ in range(content_size))
    content = Content(value=content_value)

    checksum_value = compute_checksum(content_value)
    checksum = Checksum(value=checksum_value)

    return Packet(header=header, content=content, checksum=checksum)


def get_random_req_packets(max_packets: int) -> list[Packet]:
    packets = random.randint(1, max_packets)
    return [get_random_req_packet() for _ in range(packets)]


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
        header = Header.unpack(self.recv(Header.SIZE))
        content = Content.unpack(self.recv(header.content_size))
        checksum = Checksum.unpack(self.recv(Checksum.SIZE))
        return Packet(header=header, content=content, checksum=checksum)


def main() -> None:
    with Serial() as ser:
        conn = Connection(ser=ser)
        while True:
            req_packets = get_random_req_packets(max_packets=4)
            for req_packet in req_packets:
                conn.send_packet(packet=req_packet)
            for _ in req_packets:
                conn.recv_packet()
            time.sleep(random.random())


if __name__ == "__main__":
    main()
