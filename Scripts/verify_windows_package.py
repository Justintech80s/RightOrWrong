"""Validate actual packaged files. PE checks establish architecture, not successful gameplay."""
import hashlib
from pathlib import Path
import struct


def require_win64_executable(path):
    path = Path(path)
    with path.open('rb') as stream:
        header = stream.read(64)
        if len(header) < 64 or header[:2] != b'MZ':
            raise RuntimeError(f'{path.name}: not a Windows executable')
        offset = struct.unpack_from('<I', header, 0x3C)[0]
        if offset < 64 or offset > path.stat().st_size - 26:
            raise RuntimeError(f'{path.name}: invalid PE header offset')
        stream.seek(offset)
        pe = stream.read(26)
        if pe[:4] != b'PE\0\0' or struct.unpack_from('<H', pe, 4)[0] != 0x8664:
            raise RuntimeError(f'{path.name}: not an x86-64 Windows PE')
        if struct.unpack_from('<H', pe, 24)[0] != 0x20B:
            raise RuntimeError(f'{path.name}: not a PE32+ binary')
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    return {'path': str(path), 'bytes': path.stat().st_size, 'sha256': digest.hexdigest(), 'architecture': 'AMD64_PE32_PLUS'}


def verify_package(directory):
    root = Path(directory)
    executables = list(root.rglob('RightOrWrong.exe'))
    game_binaries = [p for p in executables if p.parent.name == 'Win64' and p.parent.parent.name == 'Binaries']
    launchers = [p for p in executables if p not in game_binaries]
    if not game_binaries or not launchers:
        raise RuntimeError('Both the packaged launcher and Binaries/Win64 game executable are required')
    content = [p for p in root.rglob('*.pak') if p.stat().st_size > 0]
    if not content:
        raise RuntimeError('No cooked .pak content found; an executable alone is not the demo')
    return {'binaries': [require_win64_executable(p) for p in executables],
            'content_archives': [str(p) for p in content],
            'gameplay_verified': False, 'plugversal_verified': False}
