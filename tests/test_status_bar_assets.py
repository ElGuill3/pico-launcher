import binascii
import os
import pathlib
import shutil
import subprocess
import struct
import sys
import tempfile
import unittest
import zlib


ROOT = pathlib.Path(__file__).resolve().parents[1]
GFX = pathlib.Path(os.environ.get("STATUS_GFX_DIR", ROOT / "arm9" / "gfx"))
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
RAIL = 1
PROFILE_BACKGROUND = 12
ONLINE_BLUE = 13
BATTERY_GREEN = 14
WHITE = 15
FORMER_CHARACTER_INDICES = {8, 9, 10, 11}


def indexed_png(path):
    data = path.read_bytes()
    assert data.startswith(PNG_SIGNATURE)
    offset = len(PNG_SIGNATURE)
    chunks = {}
    while offset < len(data):
        length = struct.unpack(">I", data[offset:offset + 4])[0]
        kind = data[offset + 4:offset + 8]
        payload = data[offset + 8:offset + 8 + length]
        chunks.setdefault(kind, []).append(payload)
        offset += 12 + length

    width, height, bit_depth, color_type, compression, filter_method, interlace = struct.unpack(
        ">IIBBBBB", chunks[b"IHDR"][0])
    assert (bit_depth, color_type, compression, filter_method, interlace) == (8, 3, 0, 0, 0)
    rows = zlib.decompress(b"".join(chunks[b"IDAT"]))
    assert len(rows) == height * (width + 1)
    assert all(rows[row * (width + 1)] == 0 for row in range(height))
    pixels = [rows[row * (width + 1) + 1:(row + 1) * (width + 1)] for row in range(height)]
    return width, height, chunks[b"PLTE"][0], chunks[b"tRNS"][0], pixels


def components(points):
    remaining = set(points)
    result = []
    while remaining:
        component = {remaining.pop()}
        pending = list(component)
        while pending:
            x, y = pending.pop()
            connected = remaining & {
                (x + dx, y + dy)
                for dx in (-1, 0, 1)
                for dy in (-1, 0, 1)
                if dx or dy
            }
            remaining -= connected
            pending.extend(connected)
            component |= connected
        result.append(component)
    return result


def ds_4bpp_tiles(frames, width, height):
    encoded = bytearray()
    for frame in frames:
        for tile_y in range(0, height, 8):
            for tile_x in range(0, width, 8):
                for y in range(8):
                    row = frame[tile_y + y]
                    for x in range(0, 8, 2):
                        encoded.append(row[tile_x + x] | (row[tile_x + x + 1] << 4))
    return bytes(encoded)


def rgb555(color):
    return tuple((channel * 31 + 127) // 255 for channel in color)


def srgb_luminance(color):
    channels = [channel / 31 for channel in color]
    channels = [value / 12.92 if value <= 0.04045 else ((value + 0.055) / 1.055) ** 2.4 for value in channels]
    return 0.2126 * channels[0] + 0.7152 * channels[1] + 0.0722 * channels[2]


class StatusBarAssetTests(unittest.TestCase):
    def test_speaker_is_exactly_ten_pixels_high_with_cumulative_curved_waves(self):
        width, height, palette, transparency, pixels = indexed_png(GFX / "statusSpeaker.png")
        self.assertEqual((width, height), (16, 64))
        self.assertEqual(transparency[0], 0)
        self.assertEqual(palette[WHITE * 3:(WHITE + 1) * 3], b"\xff\xff\xff")
        self.assertEqual(set(b"".join(pixels)), {0, WHITE})

        frames = [pixels[index * 16:(index + 1) * 16] for index in range(4)]
        white = [{
            (x, y)
            for y, row in enumerate(frame)
            for x, palette_index in enumerate(row)
            if palette_index == WHITE
        } for frame in frames]
        self.assertEqual(len({b"".join(frame) for frame in frames}), 4)
        for frame in frames:
            self.assertTrue(all(frame[y] == bytes(16) for y in (0, 1, 2, 13, 14, 15)))
        for points in white:
            self.assertEqual((min(y for _, y in points), max(y for _, y in points)), (3, 12))

        bodies = [{point for point in frame if point[0] <= 6} for frame in white]
        self.assertTrue(all(body == bodies[0] for body in bodies[1:]))
        body_columns = [{y for x, y in bodies[0] if x == column} for column in range(1, 7)]
        self.assertEqual([len(column) for column in body_columns], [4, 4, 6, 8, 10, 10])
        self.assertEqual(body_columns[0], body_columns[1])
        self.assertTrue(all(column == set(range(min(column), max(column) + 1)) for column in body_columns))

        wave_pixels = [frame - bodies[0] for frame in white]
        self.assertEqual(wave_pixels[0], set())
        self.assertTrue(all(wave_pixels[index - 1] < wave_pixels[index] for index in range(1, 4)))
        introduced = [wave_pixels[index] - wave_pixels[index - 1] for index in range(1, 4)]
        self.assertTrue(all(len(components(wave)) == 1 for wave in introduced))
        self.assertEqual([len(wave) for wave in introduced], [4, 6, 8])
        self.assertEqual([max(y for _, y in wave) - min(y for _, y in wave) for wave in introduced], [3, 5, 7])
        for wave in introduced:
            top = min(y for _, y in wave)
            bottom = max(y for _, y in wave)
            middle_x = max(x for x, y in wave if y in range(7, 9))
            self.assertGreater(middle_x, max(x for x, y in wave if y in (top, bottom)))
        self.assertEqual(
            (min(x for x, _ in white[3]), max(x for x, _ in white[3])),
            (1, 14))

    def test_profile_is_a_square_lighter_tile_with_a_centered_symmetric_bust(self):
        width, height, palette, transparency, pixels = indexed_png(GFX / "statusProfile.png")
        self.assertEqual((width, height), (16, 16))
        self.assertEqual(transparency[0], 0)
        self.assertEqual(palette[PROFILE_BACKGROUND * 3:(PROFILE_BACKGROUND + 1) * 3], bytes.fromhex("56616e"))
        self.assertEqual(palette[ONLINE_BLUE * 3:(ONLINE_BLUE + 1) * 3], bytes.fromhex("66c0f4"))
        self.assertEqual(palette[WHITE * 3:(WHITE + 1) * 3], b"\xff\xff\xff")
        self.assertTrue(FORMER_CHARACTER_INDICES.isdisjoint(set(b"".join(pixels))))
        self.assertEqual(set(b"".join(pixels)), {PROFILE_BACKGROUND, ONLINE_BLUE, WHITE})
        quantized = {
            rgb555(tuple(palette[index * 3:(index + 1) * 3]))
            for index in (RAIL, PROFILE_BACKGROUND, ONLINE_BLUE, BATTERY_GREEN, WHITE)
        }
        self.assertEqual(len(quantized), 5)
        rail_luminance = srgb_luminance(rgb555(tuple(palette[RAIL * 3:(RAIL + 1) * 3])))
        profile_luminance = srgb_luminance(rgb555(tuple(palette[PROFILE_BACKGROUND * 3:(PROFILE_BACKGROUND + 1) * 3])))
        self.assertLessEqual(1.3, (profile_luminance + 0.05) / (rail_luminance + 0.05))
        self.assertLessEqual((profile_luminance + 0.05) / (rail_luminance + 0.05), 1.6)

        self.assertTrue(all(value != 0 and value != RAIL for row in pixels for value in row))
        self.assertTrue(all(row == row[::-1] for row in pixels))
        silhouette = {
            (x, y)
            for y, row in enumerate(pixels)
            for x, value in enumerate(row)
            if value in {WHITE, ONLINE_BLUE}
        }
        self.assertEqual(len(components(silhouette)), 1)
        silhouette_rows = [{x for x, value in enumerate(row) if value in {WHITE, ONLINE_BLUE}} for row in pixels]
        self.assertTrue(all(not xs or min(xs) + max(xs) == 15 for xs in silhouette_rows))

        white_rows = [{x for x, value in enumerate(row) if value == WHITE} for row in pixels]
        self.assertEqual([len(white_rows[y]) for y in range(1, 8)], [4, 6, 8, 8, 8, 6, 4])
        self.assertTrue(all(
            xs and min(xs) + max(xs) == 15
            for xs in white_rows[1:8]
        ))
        self.assertEqual([len(white_rows[y]) for y in range(8, 15)], [4, 6, 10, 6, 6, 4, 2])

        blue_rows = [{x for x, value in enumerate(row) if value == ONLINE_BLUE} for row in pixels]
        self.assertEqual([len(blue_rows[y]) for y in range(11, 15)], [6, 8, 10, 10])
        self.assertTrue(all(min(xs) + max(xs) == 15 for xs in blue_rows[11:15]))
        self.assertEqual(
            [sum(value in {WHITE, ONLINE_BLUE} for value in pixels[y]) for y in range(10, 16)],
            [10, 12, 14, 14, 12, 0],
        )

    def test_battery_frames_are_horizontal_progressive_and_distinct(self):
        width, height, palette, transparency, pixels = indexed_png(GFX / "statusBattery.png")
        self.assertEqual((width, height), (16, 88))
        self.assertEqual(transparency[0], 0)
        self.assertEqual(palette[BATTERY_GREEN * 3:(BATTERY_GREEN + 1) * 3], bytes.fromhex("6fdc50"))
        self.assertEqual(palette[WHITE * 3:(WHITE + 1) * 3], b"\xff\xff\xff")
        frames = [pixels[index * 8:(index + 1) * 8] for index in range(11)]
        self.assertEqual(len({b"".join(frame) for frame in frames}), 11)
        source_tiles = ds_4bpp_tiles(frames, 16, 8)
        self.assertEqual(len(source_tiles), 704)
        self.assertTrue(all(len(source_tiles[index * 64:(index + 1) * 64]) == 64 for index in range(11)))
        self.assertEqual(source_tiles[:64], ds_4bpp_tiles([frames[0]], 16, 8))
        self.assertEqual(set(source_tiles), {0x00, 0x0e, 0x0f, 0xee, 0xef, 0xf0, 0xfe, 0xff})

        outline = {(x, 1) for x in range(1, 13)} | {(x, 7) for x in range(1, 13)}
        outline |= {(0, y) for y in range(2, 7)} | {(13, y) for y in range(2, 7)}
        terminal = {(x, y) for x in range(14, 16) for y in range(3, 6)}
        for frame in frames:
            white = {(x, y) for y, row in enumerate(frame) for x, value in enumerate(row) if value == WHITE}
            self.assertTrue(outline <= white)
            self.assertTrue(terminal <= white)
            self.assertTrue(all(frame[y][x] == 0 for x in range(14, 16) for y in (0, 1, 2, 6, 7)))

        for frame, fill_width in zip(frames[:5], (0, 3, 6, 9, 12)):
            green = {(x, y) for y, row in enumerate(frame) for x, value in enumerate(row) if value == BATTERY_GREEN}
            self.assertEqual(green, {(x, y) for x in range(1, fill_width + 1) for y in range(2, 7)})
        self.assertEqual([sum(value == BATTERY_GREEN for row in frame for value in row) for frame in frames[:5]],
                         [0, 15, 30, 45, 60])

        charge_marker = {(6, 2), (5, 3), (6, 3), (7, 3), (6, 4), (5, 5)}
        for discharging, charging in zip(frames[:5], frames[5:10]):
            self.assertTrue(all(charging[y][x] == WHITE for x, y in charge_marker))
            self.assertNotEqual(b"".join(discharging), b"".join(charging))
        charging_green = [sum(value == BATTERY_GREEN for row in frame for value in row) for frame in frames[5:10]]
        self.assertEqual(charging_green, sorted(set(charging_green)))
        self.assertNotIn(BATTERY_GREEN, b"".join(frames[10]))
        self.assertTrue(all(frames[10][4][x] == WHITE for x in range(4, 10)))
        self.assertNotEqual(b"".join(frames[10]), b"".join(frames[0]))

    def test_only_the_three_compact_status_assets_exist(self):
        self.assertFalse((GFX / "statusRail.png").exists())
        self.assertFalse((GFX / "statusRail.grit").exists())

    def test_temp_mutants_are_rejected(self):
        if os.environ.get("STATUS_SKIP_MUTANTS"):
            self.skipTest("nested mutant run")

        battery_source = GFX / "statusBattery.png"
        profile_source = GFX / "statusProfile.png"
        _, _, _, _, battery_pixels = indexed_png(battery_source)
        _, _, _, _, profile_pixels = indexed_png(profile_source)
        battery_frames = [battery_pixels[index * 8:(index + 1) * 8] for index in range(11)]

        with self.subTest("vertical battery"), tempfile.TemporaryDirectory() as directory:
            directory = pathlib.Path(directory)
            shutil.copy2(profile_source, directory / "statusProfile.png")
            vertical_frames = [
                [bytes(frame[7 - x][y] for x in range(8)) for y in range(16)]
                for frame in battery_frames
            ]
            self.save_indexed_mutant(directory / "statusBattery.png", 8, 176, vertical_frames)
            self.assert_mutant_rejected(directory, "test_battery_frames_are_horizontal_progressive_and_distinct")

        with self.subTest("nonprogressive battery"), tempfile.TemporaryDirectory() as directory:
            directory = pathlib.Path(directory)
            shutil.copy2(profile_source, directory / "statusProfile.png")
            self.save_indexed_mutant(directory / "statusBattery.png", 16, 88, battery_frames[:1] * 5 + battery_frames[5:])
            self.assert_mutant_rejected(directory, "test_battery_frames_are_horizontal_progressive_and_distinct")

        with self.subTest("missing terminal"), tempfile.TemporaryDirectory() as directory:
            directory = pathlib.Path(directory)
            shutil.copy2(profile_source, directory / "statusProfile.png")
            frames = [[bytearray(row) for row in frame] for frame in battery_frames]
            for frame in frames:
                for y in range(3, 6):
                    frame[y][14:16] = bytes(2)
            self.save_indexed_mutant(directory / "statusBattery.png", 16, 88, frames)
            self.assert_mutant_rejected(directory, "test_battery_frames_are_horizontal_progressive_and_distinct")

        for name, mutate in {
            "profile rail background": lambda rows: [bytes(RAIL if value == PROFILE_BACKGROUND else value for value in row) for row in rows],
            "square silhouette": lambda rows: [bytes(WHITE if x < 14 else value for x, value in enumerate(row)) for row in rows],
            "asymmetric edge": lambda rows: [row[:14] + bytes([ONLINE_BLUE, PROFILE_BACKGROUND]) for row in rows],
        }.items():
            with self.subTest(name), tempfile.TemporaryDirectory() as directory:
                directory = pathlib.Path(directory)
                shutil.copy2(battery_source, directory / "statusBattery.png")
                self.save_indexed_mutant(directory / "statusProfile.png", 16, 16, mutate(profile_pixels))
                self.assert_mutant_rejected(
                    directory, "test_profile_is_a_square_lighter_tile_with_a_centered_symmetric_bust")

    @staticmethod
    def save_indexed_mutant(path, width, height, rows):
        rows = [row for group in rows for row in group] if rows and isinstance(rows[0], list) else rows
        palette = bytearray(bytes.fromhex("000000") * 16)
        for index, color in {
            RAIL: "3f4854", PROFILE_BACKGROUND: "56616e", ONLINE_BLUE: "66c0f4",
            BATTERY_GREEN: "6fdc50", WHITE: "ffffff",
        }.items():
            palette[index * 3:(index + 1) * 3] = bytes.fromhex(color)

        def chunk(kind, payload):
            return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", binascii.crc32(kind + payload))

        raw = b"".join(b"\0" + bytes(row) for row in rows)
        data = PNG_SIGNATURE
        data += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 3, 0, 0, 0))
        data += chunk(b"PLTE", bytes(palette))
        data += chunk(b"tRNS", bytes([0] + [255] * 15))
        data += chunk(b"IDAT", zlib.compress(raw))
        data += chunk(b"IEND", b"")
        path.write_bytes(data)

    def assert_mutant_rejected(self, directory, test_name):
        result = subprocess.run(
            [sys.executable, str(pathlib.Path(__file__).resolve()), f"StatusBarAssetTests.{test_name}"],
            env={**os.environ, "STATUS_GFX_DIR": str(directory), "STATUS_SKIP_MUTANTS": "1"},
            capture_output=True,
            text=True,
        )
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
