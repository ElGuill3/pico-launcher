import os
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
CONVERTER = ROOT / "tools" / "convert_intro.py"


class ConvertIntroTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.directory = Path(self.temp.name)

    def tearDown(self):
        self.temp.cleanup()

    def make_animation(self, name, frame_count=5, duration=100, size=(256, 192)):
        frames = [Image.new("RGBA", size, ((index * 20) % 256, 0, 0, 128)) for index in range(frame_count)]
        path = self.directory / name
        frames[0].save(path, save_all=True, append_images=frames[1:], duration=duration, loop=0)
        return path

    def convert(self, source, output="intro.pani"):
        destination = self.directory / output
        result = subprocess.run(
            [sys.executable, str(CONVERTER), str(source), str(destination)],
            text=True, capture_output=True,
        )
        return result, destination

    def test_gif_and_apng_are_deterministic_valid_lz4_packages(self):
        for extension in ("gif", "png"):
            with self.subTest(extension=extension):
                source = self.make_animation(f"source.{extension}")
                first, first_path = self.convert(source, f"first-{extension}.pani")
                second, second_path = self.convert(source, f"second-{extension}.pani")
                self.assertEqual(first.returncode, 0, first.stderr)
                self.assertEqual(second.returncode, 0, second.stderr)
                data = first_path.read_bytes()
                self.assertEqual(data, second_path.read_bytes())
                header = struct.unpack_from("<4sHHHHHHIIII", data)
                self.assertEqual(header[:7], (b"PANI", 1, 32, 256, 192, 5, 1))
                self.assertEqual(header[7], 32)
                self.assertEqual(header[8], 544)
                self.assertEqual(header[9], len(data))
                self.assertLessEqual(len(data), 4 * 1024 * 1024)
                entries = [struct.unpack_from("<IIHH", data, 544 + index * 12) for index in range(5)]
                self.assertEqual([entry[2] for entry in entries], [6] * 5)
                self.assertTrue(all(0 < entry[1] < 49152 for entry in entries))
                self.assertTrue(all(entry[0] % 512 == 0 for entry in entries))
                table_end = 544 + len(entries) * 12
                self.assertEqual(data[table_end:entries[0][0]], bytes(entries[0][0] - table_end))
                for offset, length, _, _ in entries:
                    padded_end = offset + ((length + 511) & ~511)
                    self.assertLessEqual(padded_end, len(data))
                    self.assertEqual(data[offset + length:padded_end], bytes(padded_end - offset - length))
                self.assertEqual(len(data), entries[-1][0] + ((entries[-1][1] + 511) & ~511))
                cpp_test = os.environ.get("PANI_TEST_BINARY")
                if cpp_test:
                    subprocess.run([cpp_test, str(first_path)], check=True)

    def assert_rejected(self, source, message):
        result, destination = self.convert(source)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(message, result.stderr)
        self.assertFalse(destination.exists())

    def test_rejects_wrong_dimensions(self):
        self.assert_rejected(self.make_animation("wrong.gif", size=(16, 16)), "exactly 256x192")

    def test_rejects_static_input(self):
        source = self.directory / "static.png"
        Image.new("RGB", (256, 192)).save(source)
        self.assert_rejected(source, "animated")

    def test_rejects_fast_frames(self):
        self.assert_rejected(self.make_animation("fast.gif", duration=50), "at least 67 ms")

    def test_rejects_short_total_duration(self):
        self.assert_rejected(self.make_animation("short.gif", frame_count=2), "30..600 ticks")

    def test_rejects_long_total_duration(self):
        self.assert_rejected(self.make_animation("long.gif", frame_count=101), "30..600 ticks")

    def test_rejects_too_many_frames(self):
        self.assert_rejected(self.make_animation("many.gif", frame_count=151), "1..150 frames")

    def test_rejects_unsupported_extension(self):
        source = self.directory / "source.jpg"
        Image.new("RGB", (256, 192)).save(source)
        self.assert_rejected(source, ".gif or animated .png/.apng")


if __name__ == "__main__":
    unittest.main()
