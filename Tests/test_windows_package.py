import importlib.util
from pathlib import Path
import struct
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('package_checks', ROOT / 'Scripts/verify_windows_package.py')
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class PackageValidationTests(unittest.TestCase):
    # Minimal headers are fixtures for the parser, never distributable executables.
    def fixture(self, machine=0x8664, magic=0x20B):
        data = bytearray(256)
        data[:2] = b'MZ'
        struct.pack_into('<I', data, 0x3C, 128)
        data[128:132] = b'PE\0\0'
        struct.pack_into('<H', data, 132, machine)
        struct.pack_into('<H', data, 152, magic)
        return data

    def test_pe64_header_is_recognized(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'header.bin'
            path.write_bytes(self.fixture())
            result = module.require_win64_executable(path)
            self.assertEqual(result['architecture'], 'AMD64_PE32_PLUS')
            self.assertEqual(len(result['sha256']), 64)

    def test_wrong_architecture_and_invalid_headers_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / 'header.bin'
            for data in [b'', b'not a binary', self.fixture(machine=0x14c), self.fixture(machine=0xAA64), self.fixture(magic=0x10B)]:
                path.write_bytes(data)
                with self.assertRaises(RuntimeError):
                    module.require_win64_executable(path)

    def test_missing_content_and_game_binary_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'RightOrWrong.exe').write_bytes(self.fixture())
            with self.assertRaises(RuntimeError):
                module.verify_package(root)
            binary = root / 'RightOrWrong/Binaries/Win64/RightOrWrong.exe'
            binary.parent.mkdir(parents=True)
            binary.write_bytes(self.fixture())
            with self.assertRaises(RuntimeError):
                module.verify_package(root)

    def test_file_checks_never_claim_gameplay_or_launcher_compatibility(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'RightOrWrong.exe').write_bytes(self.fixture())
            binary = root / 'RightOrWrong/Binaries/Win64/RightOrWrong.exe'
            binary.parent.mkdir(parents=True)
            binary.write_bytes(self.fixture())
            (root / 'cooked.pak').write_bytes(b'test fixture, not game content')
            result = module.verify_package(root)
            self.assertFalse(result['gameplay_verified'])
            self.assertFalse(result['plugversal_verified'])


if __name__ == '__main__':
    unittest.main()
