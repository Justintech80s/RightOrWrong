import json
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
data = json.loads((ROOT/'Docs/DEMO_MISSION_PACK_v05.json').read_text())
missions = data['missions']
ids = {m['id'] for m in missions}
assert len(ids) == len(missions) == 3
first = missions[0]
assert first['program_choices'] == ['Karate','Judo','Taekwondo','Archery','Fencing','Sport Shooting']
assert any('selected program' in x for x in first['objectives'])
assert not any('karate' in x.lower() for x in first['objectives'])
assert {c['id'] for c in first['choices']} == {'report','protect'}
for mission in missions:
    assert all(dep in ids for dep in mission['requires'])
    assert len(mission['choices']) >= 2
print('PASS: mission data preserves free six-sport selection and the recruit moral decision.')
