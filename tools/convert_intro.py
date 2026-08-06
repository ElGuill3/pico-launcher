#!/usr/bin/env python3
import argparse
import os
import struct
import sys
import tempfile
from pathlib import Path

import lz4.block
from PIL import Image, ImageSequence


WIDTH = 256
HEIGHT = 192
FRAME_BYTES = WIDTH * HEIGHT
HEADER_SIZE = 32
PALETTE_BYTES = 512
ENTRY_BYTES = 12
MAX_FRAMES = 150
MAX_FILE_BYTES = 4 * 1024 * 1024
SECTOR_BYTES = 512


def sector_bytes(size):
    return (size + SECTOR_BYTES - 1) & ~(SECTOR_BYTES - 1)


def load_frames(path):
    if path.suffix.lower() not in {".gif", ".png", ".apng"}:
        raise ValueError("input must be .gif or animated .png/.apng")

    with Image.open(path) as image:
        if not getattr(image, "is_animated", False) or image.n_frames < 2:
            raise ValueError("input must be animated")
        if image.n_frames > MAX_FRAMES:
            raise ValueError("input must contain 1..150 frames")
        if image.size != (WIDTH, HEIGHT):
            raise ValueError("input dimensions must be exactly 256x192")

        frames = []
        ticks = []
        for frame in ImageSequence.Iterator(image):
            if frame.size != (WIDTH, HEIGHT):
                raise ValueError("every frame must be exactly 256x192")
            duration = frame.info.get("duration", image.info.get("duration", 0))
            if duration < 67:
                raise ValueError("every frame duration must be at least 67 ms")
            rgba = frame.convert("RGBA")
            black = Image.new("RGBA", rgba.size, (0, 0, 0, 255))
            frames.append(Image.alpha_composite(black, rgba).convert("RGB"))
            ticks.append(max(4, int(duration * 60 / 1000 + 0.5)))

    if not 30 <= sum(ticks) <= 600:
        raise ValueError("total duration must be 30..600 ticks (0.5..10 seconds)")
    return frames, ticks


def encode(frames, ticks):
    atlas = Image.new("RGB", (WIDTH, HEIGHT * len(frames)))
    for index, frame in enumerate(frames):
        atlas.paste(frame, (0, index * HEIGHT))
    quantized = atlas.quantize(colors=256, method=Image.Quantize.MEDIANCUT,
        dither=Image.Dither.NONE)
    palette = quantized.getpalette()
    palette += [0] * (768 - len(palette))
    palette_image = Image.new("P", (1, 1))
    palette_image.putpalette(palette)
    palette_data = bytearray()
    for index in range(256):
        red, green, blue = palette[index * 3:index * 3 + 3]
        palette_data += struct.pack("<H", (red >> 3) | ((green >> 3) << 5) | ((blue >> 3) << 10))

    payloads = []
    for frame in frames:
        indexed = frame.quantize(palette=palette_image, dither=Image.Dither.NONE).tobytes()
        compressed = lz4.block.compress(indexed, mode="high_compression", store_size=False)
        if len(compressed) >= FRAME_BYTES:
            raise ValueError("LZ4 compression must be smaller than RAW for every frame")
        payloads.append(compressed)

    table_offset = HEADER_SIZE + PALETTE_BYTES
    table_end = table_offset + len(frames) * ENTRY_BYTES
    payload_offset = sector_bytes(table_end)
    entries = []
    stored_payloads = []
    offset = payload_offset
    for payload, duration in zip(payloads, ticks):
        entries.append(struct.pack("<IIHH", offset, len(payload), duration, 0))
        stored_length = sector_bytes(len(payload))
        stored_payloads.append(payload + bytes(stored_length - len(payload)))
        offset += stored_length
    if offset > MAX_FILE_BYTES:
        raise ValueError("generated PANI must not exceed 4 MiB")

    header = struct.pack("<4sHHHHHHIIII", b"PANI", 1, HEADER_SIZE, WIDTH, HEIGHT,
        len(frames), 1, HEADER_SIZE, table_offset, offset, 0)
    metadata = header + palette_data + b"".join(entries)
    return metadata + bytes(payload_offset - len(metadata)) + b"".join(stored_payloads)


def write_atomic(path, data):
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=path.parent, prefix=f".{path.name}.", delete=False) as output:
            temporary = Path(output.name)
            output.write(data)
            output.flush()
            os.fsync(output.fileno())
        os.replace(temporary, path)
    finally:
        if temporary and temporary.exists():
            temporary.unlink()


def main():
    parser = argparse.ArgumentParser(description="Convert GIF/APNG to Pico Launcher PANI")
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    arguments = parser.parse_args()
    try:
        frames, ticks = load_frames(arguments.input)
        write_atomic(arguments.output, encode(frames, ticks))
    except (OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
