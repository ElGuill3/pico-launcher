#!/usr/bin/env python3
"""Build the pinned Noto Sans JP Medium 9 NFT2 status font."""
import argparse
import hashlib
import pathlib
import struct
import urllib.request

URL = "https://raw.githubusercontent.com/notofonts/noto-cjk/523d033d6cb47f4a80c58a35753646f5c3608a78/Sans/SubsetOTF/JP/NotoSansJP-Medium.otf"
BLOB = "490cc708a90767e9a9c867e147df33a8c62c1f6d"
SIZE, GLYPHS, GLYPH_OFFSET, CHARMAP_OFFSET, DATA_OFFSET = 4549132, 420, 0x14, 0x0D34, 0x115C
PIXEL_SIZE, PILLOW_VERSION = 9, "11.1.0"
ROOT = pathlib.Path(__file__).resolve().parents[1]
CANONICAL = ROOT / "arm9/data/NotoSansJP-Medium-10.nft2"


def blob_hash(data):
    return hashlib.sha1(f"blob {len(data)}\0".encode() + data).hexdigest()


def font_bytes(path):
    if not path.exists():
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(urllib.request.urlopen(URL).read())
    data = path.read_bytes()
    if len(data) != SIZE or blob_hash(data) != BLOB:
        raise ValueError("pinned NotoSansJP-Medium.otf hash mismatch")
    return path


def glyph_map(data):
    offset = struct.unpack_from("<I", data, 8)[0]
    result = [None] * GLYPHS
    while (count := struct.unpack_from("<H", data, offset)[0]):
        start = struct.unpack_from("<H", data, offset + 2)[0]
        for character, glyph in enumerate(struct.unpack_from(f"<{count}H", data, offset + 4), start):
            if result[glyph] is None:
                result[glyph] = character
        offset += 4 + count * 2
    return result


def metrics(data, text):
    glyphs, charmap, base = struct.unpack_from("<III", data, 4)
    x = 0
    for character in text:
        offset = charmap
        glyph = 0
        while (count := struct.unpack_from("<H", data, offset)[0]):
            start = struct.unpack_from("<H", data, offset + 2)[0]
            if start <= ord(character) < start + count:
                glyph = struct.unpack_from("<H", data, offset + 4 + 2 * (ord(character) - start))[0]
                break
            offset += 4 + count * 2
        data_offset, left, right, _, _ = struct.unpack_from("<IbbBb", data, glyphs + glyph * 8)
        width = data_offset >> 24
        del data_offset
        x += left + width + right
    return x


def validate_nft2(data):
    signature, glyphs, charmap, base, ascend, descend, count = struct.unpack_from("<IIII BBH", data)
    if signature != 0x3254464E or (glyphs, charmap, base, count) != (GLYPH_OFFSET, CHARMAP_OFFSET, DATA_OFFSET, GLYPHS):
        raise ValueError("invalid Medium9 NFT2 layout")
    if data[charmap:base] != CANONICAL.read_bytes()[CHARMAP_OFFSET:DATA_OFFSET]:
        raise ValueError("canonical character map changed")
    for offset in range(glyphs, glyphs + count * 8, 8):
        data_offset, _, _, height, _ = struct.unpack_from("<IbbBb", data, offset)
        width, data_offset = data_offset >> 24, data_offset & 0x00ffffff
        if data_offset + ((width + 1) // 2) * height > len(data) - base:
            raise ValueError("glyph data out of range")
    return {"ascend": ascend, "descend": descend, "glyph_count": count, "glyph_offset": glyphs,
            "charmap_offset": charmap, "data_offset": base,
            "widths": {sample: metrics(data, sample) for sample in ("12/31 23:59", "HIGH", "LOW", "WWWW ... W")}}


def build(source):
    from PIL import Image, ImageDraw, ImageFont, __version__
    if __version__ != PILLOW_VERSION:
        raise RuntimeError(f"Pillow {PILLOW_VERSION} required, got {__version__}")
    canonical = CANONICAL.read_bytes()
    font = ImageFont.truetype(str(source), PIXEL_SIZE, layout_engine=ImageFont.Layout.BASIC)
    records, pixels = bytearray(), bytearray()
    for character in glyph_map(canonical):
        box = font.getbbox(chr(character))
        left, top, right, bottom = box
        width, height = right - left, bottom - top
        advance = round(font.getlength(chr(character)))
        records += struct.pack("<IbbBb", len(pixels) | (width << 24), left, advance - left - width, height, top)
        image = Image.new("L", (width, height))
        ImageDraw.Draw(image).text((-left, -top), chr(character), font=font, fill=255)
        values = [(value * 15 + 127) // 255 for value in image.getdata()]
        for row in range(height):
            for column in range(0, width, 2):
                pixels.append(values[row * width + column] | (values[row * width + column + 1] << 4 if column + 1 < width else 0))
    ascend, descend = font.getmetrics()
    result = bytearray(struct.pack("<IIII BBH", 0x3254464E, GLYPH_OFFSET, CHARMAP_OFFSET, DATA_OFFSET, ascend, descend, GLYPHS))
    result += records + canonical[CHARMAP_OFFSET:DATA_OFFSET] + pixels
    return bytes(result)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=pathlib.Path, default=ROOT / "arm9/data/NotoSansJP-Medium-9.nft2")
    parser.add_argument("--cache", type=pathlib.Path, default=pathlib.Path("/tmp/pico-launcher-font-cache/NotoSansJP-Medium.otf"))
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()
    if args.validate:
        print(validate_nft2(args.output.read_bytes()))
    else:
        args.output.write_bytes(build(font_bytes(args.cache)))
        print(validate_nft2(args.output.read_bytes()))


if __name__ == "__main__":
    main()
