"""Run INSIDE Unreal Editor's Python runtime. Creates missing demo assets without overwriting existing ones."""
from pathlib import Path
import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
material_path = '/Game/Materials/M_ROWColor'
if not unreal.EditorAssetLibrary.does_asset_exist(material_path):
    material = asset_tools.create_asset('M_ROWColor', '/Game/Materials', unreal.Material, unreal.MaterialFactoryNew())
    if not material:
        raise RuntimeError('Could not create the uniform/target material')
    color = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionVectorParameter, -300, 0)
    color.set_editor_property('parameter_name', 'Tint')
    color.set_editor_property('default_value', unreal.LinearColor(0.85, 0.55, 0.08, 1.0))
    if not unreal.MaterialEditingLibrary.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR):
        raise RuntimeError('Could not connect the color parameter')
    unreal.MaterialEditingLibrary.recompile_material(material)
    if not unreal.EditorAssetLibrary.save_loaded_asset(material):
        raise RuntimeError('Could not save the demo material')

level_path = '/Game/Maps/ROW_City'
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not unreal.EditorAssetLibrary.does_asset_exist(level_path):
    if not levels.new_level(level_path):
        raise RuntimeError('Could not create the demo map')
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    start = actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, 0, 150))
    if not start or not levels.save_current_level():
        raise RuntimeError('Could not save the player start and demo map')

config = Path(unreal.Paths.project_config_dir()) / 'DefaultEngine.ini'
content = config.read_text(encoding='utf-8')
lines = content.splitlines()
section = '[/Script/EngineSettings.GameMapsSettings]'
start_index = lines.index(section)
end_index = next((i for i in range(start_index + 1, len(lines)) if lines[i].startswith('[')), len(lines))
replacement = [line for line in lines[start_index + 1:end_index] if not line.startswith('EditorStartupMap=')]
replacement.insert(0, 'EditorStartupMap=/Game/Maps/ROW_City')
lines[start_index + 1:end_index] = replacement
config.write_text('\n'.join(lines) + '\n', encoding='utf-8')
unreal.log('RIGHT OR WRONG: demo map and color material are present. Runtime world is spawned by ROWGameMode.')
