#!/usr/bin/env python3
"""Run local source/rules tests and record their actual results separately from Unreal validation."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
ROOT = Path(__file__).resolve().parents[1]

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx',default=shutil.which('g++') or shutil.which('clang++'))
    parser.add_argument('--sanitize',action='store_true')
    args=parser.parse_args()
    if not args.cxx: parser.error('A C++17 compiler (g++ or clang++) is required for the standalone rule tests.')
    evidence=ROOT/'BuildEvidence';evidence.mkdir(exist_ok=True)
    results=[]
    commands=[('source_preflight',[sys.executable,'Scripts/verify_source.py']),('mission_data',[sys.executable,'Scripts/verify_demo_missions.py']),('pipeline_safeguards',[sys.executable,'-m','unittest','discover','-s','Tests','-p','test_*.py'])]
    def run(name,command):
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=120)
        (evidence/(name+'.log')).write_text(result.stdout+result.stderr,encoding='utf-8')
        results.append({'name':name,'command':command,'exit_code':result.returncode,'status':'PASS' if result.returncode==0 else 'FAIL'})
        print(name,results[-1]['status'])
        return result.returncode==0
    with tempfile.TemporaryDirectory() as temp:
        for name,cmd in commands: run(name,cmd)
        for source in ['Scripts/verify_rules.cpp','Scripts/verify_pedagogy.cpp','Tests/story_routing_test.cpp','Tests/narrative_demo_test.cpp','Tests/combat_demo_test.cpp','Tests/blockout_test.cpp']:
            name=Path(source).stem
            target=Path(temp)/(name+('.exe' if sys.platform=='win32' else ''))
            flags=['-std=c++17','-Wall','-Wextra','-Werror','-pedantic']
            if args.sanitize: flags+=['-fsanitize=undefined','-fno-sanitize-recover=all']
            if run(name+'_compile',[args.cxx,*flags,source,'-o',str(target)]): run(name,[str(target)])
    passed=all(r['status']=='PASS' for r in results)
    report={'edition':'Eight-Stage Demo Source 1','baseline':'recovered v0.6; v2.1 unavailable','local_checks':'PASS' if passed else 'FAIL','results':results,'unreal_compile':'NOT_RUN_ENGINE_UNAVAILABLE','unreal_automation':'NOT_RUN_ENGINE_UNAVAILABLE','visual_playtest':'NOT_RUN','windows_package':'NOT_CREATED'}
    (evidence/'Verification.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    return 0 if passed else 1
if __name__=='__main__':sys.exit(main())
