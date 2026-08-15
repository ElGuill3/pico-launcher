import importlib.util
import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]
GENERATOR = ROOT / "tools" / "generate_medium9_font.py"
ASSET = ROOT / "arm9" / "data" / "NotoSansJP-Medium-9.nft2"


class Medium9FontTests(unittest.TestCase):
    def test_generated_asset_preserves_the_nft2_contract(self):
        spec = importlib.util.spec_from_file_location("medium9", GENERATOR)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        info = module.validate_nft2(ASSET.read_bytes())
        self.assertEqual(info["glyph_count"], 420)
        self.assertEqual(info["glyph_offset"], 0x14)
        self.assertEqual(info["charmap_offset"], 0x0D34)
        self.assertEqual(info["data_offset"], 0x115C)
        self.assertEqual((info["ascend"], info["descend"]), (11, 3))
        self.assertLessEqual(info["ascend"] + info["descend"], 16)
        self.assertEqual(info["widths"], {"12/31 23:59": 49, "HIGH": 23, "LOW": 20, "WWWW ... W": 53})
        self.assertLessEqual(info["widths"]["12/31 23:59"], 64)
        self.assertLessEqual(info["widths"]["HIGH"], 24)
        self.assertLessEqual(info["widths"]["LOW"], 24)
        self.assertLessEqual(info["widths"]["WWWW ... W"], 66)


if __name__ == "__main__":
    unittest.main()
