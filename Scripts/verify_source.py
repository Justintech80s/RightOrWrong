"""Preflight source/configuration checks. This does not compile Unreal or verify gameplay."""
import json
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
MOD = ROOT / 'Source/RightOrWrong'
checks = 0

def check(label, condition):
    global checks
    if not condition:
        raise AssertionError(label)
    checks += 1
    print('PASS:', label)

project = json.loads((ROOT/'RightOrWrong.uproject').read_text())
check('Declared runtime module', any(x['Name'] == 'RightOrWrong' for x in project['Modules']))
check('Declared engine target', project['EngineAssociation'] == '5.8')
for cpp in MOD.rglob('*.cpp'):
    text = cpp.read_text()
    sibling = cpp.with_suffix('.h')
    if sibling.exists():
        first = re.search(r'^#include "([^"]+)"', text, re.M)
        check(cpp.name + ' matching header first', bool(first) and first.group(1) == sibling.name)
    for header in re.findall(r'#include "(ROW[^"/\r\n]+\.h)"', text):
        check(cpp.name + ' local dependency ' + header, (MOD/header).is_file())
    stripped = re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', '', text, flags=re.S)
    stack = []
    for char in stripped:
        if char in '([{': stack.append(char)
        elif char in ')]}':
            if not stack or stack.pop() != '([{'[')]}'.index(char)]:
                raise AssertionError(cpp.name + ' unbalanced delimiter')
    check(cpp.name + ' all delimiters closed', not stack)
for header in MOD.glob('*.h'):
    text = header.read_text()
    if 'UCLASS(' in text:
        includes = re.findall(r'#include "([^"]+)"', text)
        check(header.name + ' generated header last', includes[-1] == header.stem + '.generated.h')
        check(header.name + ' reflected body', 'GENERATED_BODY()' in text)
config = (ROOT/'Config/DefaultInput.ini').read_text()
character = (MOD/'ROWCharacter.cpp').read_text()
for action in ['Interact','Attack','Block','Save','Load','ChoiceOne','ChoiceTwo','ChoiceThree','ChoiceFour','ChoiceFive','ChoiceSix','PauseMenu','Journal','Advance']:
    check('Input ' + action, f'ActionName="{action}"' in config and f'BindAction(TEXT("{action}")' in character)
engine = (ROOT/'Config/DefaultEngine.ini').read_text()
check('Configured game map', '\nGameDefaultMap=/Game/Maps/ROW_City\n' in engine)
check('Configured game mode', 'GlobalDefaultGameMode=/Script/RightOrWrong.ROWGameMode' in engine)
check('Slate dependencies', all(f'"{name}"' in (MOD/'RightOrWrong.Build.cs').read_text() for name in ['Slate','SlateCore']))
check('No spoken-dialogue playback in narrative or UI', not any(token in ((MOD/'ROWNarrative.h').read_text()+(MOD/'ROWHUD.cpp').read_text()) for token in ['PlaySound','SpawnSound','SpeechSynthesis','TextToSpeech']))
director = (MOD/'ROWWorldDirector.cpp').read_text()
check('AI opponent is instantiated', 'SpawnActor<AROWOpponent>' in director)
check('Live combat uses tested match model', all(token in director for token in ['Combat.Tick(', 'Combat.Strike(', 'CombatFrame()']))
check('Runtime blockout shares checked geometry', 'ROWBlockout::Geometry()' in director)
check('Checkpoint fallback retains older save', all(token in director for token in ['BackupSlot','ValidateSave','Newest==0?1:0','ReadBack']))
check('Opponent uses swept character movement', 'AddMovementInput' in (MOD/'ROWOpponent.cpp').read_text() and 'public ACharacter' in (MOD/'ROWOpponent.h').read_text())
print(f'PASS: {checks} preflight assertions. Unreal compilation and graphics testing are not part of this script.')
