import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('build_windows', ROOT/'Scripts/build_windows.py')
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
class BuildEvidenceTests(unittest.TestCase):
    def check(self, data):
        with tempfile.TemporaryDirectory() as temp:
            file = Path(temp)/'index.json'
            file.write_text(json.dumps(data))
            module.check_automation(file)
    def successes(self):
        return [{'fullTestPath':name,'state':'Success'} for name in module.REQUIRED_TESTS]
    def test_missing_combat_test_blocks_packaging(self):
        with self.assertRaises(RuntimeError):
            self.check({'tests':[self.successes()[0]]})
    def test_required_test_passes(self):
        self.check({'tests':self.successes(), 'failed':0,'notRun':0})
    def test_empty_report_fails(self):
        with self.assertRaises(RuntimeError): self.check({'tests':[]})
    def test_failed_required_test_fails(self):
        with self.assertRaises(RuntimeError): self.check({'tests':[{'fullTestPath':module.REQUIRED_TESTS[0],'state':'Fail'}]})
    def test_other_failures_block_packaging(self):
        with self.assertRaises(RuntimeError): self.check({'tests':self.successes(), 'failed':1})
    def test_skipped_tests_block_packaging(self):
        with self.assertRaises(RuntimeError): self.check({'tests':self.successes(), 'notRun':1})
    def test_paths_remain_separate_arguments(self):
        steps = module.commands(Path('C:/Program Files/Epic Games/UE_5.8'),Path('C:/Game Builds/ROW'))
        self.assertEqual([x[0] for x in steps], ['editor_compile','asset_generation','engine_automation','windows_package'])
        self.assertIn('Program Files',steps[0][1][0])
        self.assertIn('-map=/Game/Maps/ROW_City',steps[-1][1])
if __name__ == '__main__': unittest.main()
