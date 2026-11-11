#!/usr/bin/env python3
"""Compile, generate assets, test Unreal serialization, then package. Never treats a skipped stage as passed."""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT / 'RightOrWrong.uproject'
REQUIRED_TESTS = ['RightOrWrong.Narrative.SaveGameRoundTrip', 'RightOrWrong.Combat.SaveGameRoundTrip']
spec = importlib.util.spec_from_file_location('row_package_checks', ROOT / 'Scripts/verify_windows_package.py')
package_checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(package_checks)

def commands(engine, output):
    project = str(PROJECT)
    reports = output / 'Automation'
    return [
        ('editor_compile', [str(engine / 'Engine/Build/BatchFiles/Build.bat'), 'RightOrWrongEditor', 'Win64', 'Development', project, '-WaitMutex', '-NoHotReloadFromIDE']),
        ('asset_generation', [str(engine / 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'), project, '-unattended', '-nop4', '-nosplash', '-NullRHI', '-ExecutePythonScript=' + str(ROOT / 'Scripts/setup_demo.py')]),
        ('engine_automation', [str(engine / 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'), project, '/Game/Maps/ROW_City', '-unattended', '-nop4', '-NullRHI', '-nosound', '-ExecCmds=Automation RunTests RightOrWrong.', '-TestExit=Automation Test Queue Empty', '-ReportExportPath=' + str(reports)]),
        ('windows_package', [str(engine / 'Engine/Build/BatchFiles/RunUAT.bat'), 'BuildCookRun', '-project=' + project, '-noP4', '-platform=Win64', '-clientconfig=Development', '-build', '-cook', '-stage', '-pak', '-prereqs', '-archive', '-archivedirectory=' + str(output / 'WindowsDemo'), '-map=/Game/Maps/ROW_City', '-utf8output']),
    ]

def check_automation(report_path):
    data = json.loads(report_path.read_text(encoding='utf-8-sig'))
    tests = data.get('tests', [])
    for required in REQUIRED_TESTS:
        selected = [t for t in tests if t.get('fullTestPath') == required or t.get('testDisplayName') == required]
        if not selected or any(t.get('state') != 'Success' for t in selected):
            raise RuntimeError('Required Unreal test is absent or did not succeed: ' + required)
    if data.get('failed', 0) or data.get('notRun', 0) or data.get('inProcess', 0):
        raise RuntimeError('Unreal reports failed, skipped or unfinished tests')

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--engine-root', default=os.environ.get('UE_ROOT'))
    ap.add_argument('--output', type=Path, default=ROOT / 'BuildOutput')
    ap.add_argument('--plan', action='store_true', help='Print commands only; does not validate or build')
    ap.add_argument('--compile-only', action='store_true')
    args = ap.parse_args()
    if not args.engine_root:
        ap.error('Set --engine-root to the installed UE 5.8 folder (or set UE_ROOT).')
    engine = Path(args.engine_root).resolve()
    output = args.output.resolve()
    steps = commands(engine, output)
    if args.compile_only:
        steps = steps[:1]
    if args.plan:
        print(json.dumps({'status': 'PLAN_ONLY_NOT_EXECUTED', 'steps': steps}, indent=2))
        return 0
    if os.name != 'nt':
        raise RuntimeError('Windows build stages require a Windows machine with Unreal and its C++ toolchain.')
    version_file = engine / 'Engine/Build/Build.version'
    version = json.loads(version_file.read_text(encoding='utf-8-sig'))
    expected = json.loads(PROJECT.read_text())['EngineAssociation']
    if f"{version['MajorVersion']}.{version['MinorVersion']}" != expected:
        raise RuntimeError(f'Project requests Unreal {expected}; the supplied installation differs.')
    for _, command in steps:
        if not Path(command[0]).is_file():
            raise FileNotFoundError(command[0])
    if not args.compile_only and (output / 'WindowsDemo').exists() and any((output / 'WindowsDemo').iterdir()):
        raise RuntimeError('The package destination is not empty. Choose a new --output directory so stale builds cannot be accepted.')
    output.mkdir(parents=True, exist_ok=True)
    summary = {'status': 'IN_PROGRESS', 'engine': version, 'stages': [], 'playable_gameplay_verified': False}
    evidence = output / 'pipeline_result.json'
    try:
        for name, command in steps:
            started = time.time()
            if name == 'engine_automation' and (output / 'Automation/index.json').exists():
                # Never accept a report from an earlier run.
                (output / 'Automation/index.json').unlink()
            with (output / (name + '.log')).open('w', encoding='utf-8') as log:
                result = subprocess.run(command, cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, timeout=7200, check=False)
            summary['stages'].append({'name': name, 'exit_code': result.returncode, 'seconds': round(time.time()-started,2)})
            if result.returncode:
                raise RuntimeError(f'{name} failed; inspect {name}.log')
            if name == 'asset_generation':
                for asset in ['Content/Maps/ROW_City.umap', 'Content/Materials/M_ROWColor.uasset']:
                    if not (ROOT / asset).is_file():
                        raise RuntimeError('Asset generation did not produce ' + asset)
            if name == 'engine_automation':
                check_automation(output / 'Automation/index.json')
            if name == 'windows_package':
                summary['package_files'] = package_checks.verify_package(output / 'WindowsDemo')
            evidence.write_text(json.dumps(summary, indent=2), encoding='utf-8')
        summary['status'] = 'COMPILE_PASSED' if args.compile_only else 'PACKAGE_CREATED_GAMEPLAY_REVIEW_REQUIRED'
    except Exception as exc:
        summary['status'] = 'FAILED'
        summary['error'] = str(exc)
        raise
    finally:
        evidence.write_text(json.dumps(summary, indent=2), encoding='utf-8')
    print(json.dumps(summary, indent=2))
    return 0

if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as error:
        print('BLOCKED/FAILED:', error, file=sys.stderr)
        sys.exit(1)
