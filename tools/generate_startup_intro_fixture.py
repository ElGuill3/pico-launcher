#!/usr/bin/env python3
import struct
import sys

WIDTH = 256
HEIGHT = 192
FRAME_BYTES = WIDTH * HEIGHT
FRAME_COUNT = 3
HEADER_SIZE = 32
PALETTE_SIZE = 512
ENTRY_SIZE = 12


def main() -> None:
    output = sys.argv[1] if len(sys.argv) > 1 else "intro.pani"
    table_offset = HEADER_SIZE + PALETTE_SIZE
    frames_offset = table_offset + FRAME_COUNT * ENTRY_SIZE
    file_size = frames_offset + FRAME_COUNT * FRAME_BYTES

    header = struct.pack(
        "<4sHHHHHHIIII",
        b"PANI", 1, HEADER_SIZE, WIDTH, HEIGHT, FRAME_COUNT, 0,
        HEADER_SIZE, table_offset, file_size, 0,
    )
    palette = [0x0000, 0x001F, 0x03E0, 0x7C00] + [0] * 252
    table = b"".join(
        struct.pack("<IIHH", frames_offset + index * FRAME_BYTES, FRAME_BYTES, 15, 0)
        for index in range(FRAME_COUNT)
    )

    frames = []
    for index in range(FRAME_COUNT):
        frame = bytearray(FRAME_BYTES)
        left = 48 + index * 64
        for y in range(48, 144):
            frame[y * WIDTH + left:y * WIDTH + left + 32] = bytes([index + 1]) * 32
        frames.append(frame)

    with open(output, "wb") as fixture:
        fixture.write(header)
        fixture.write(struct.pack("<256H", *palette))
        fixture.write(table)
        fixture.write(b"".join(frames))


if __name__ == "__main__":
    main()
