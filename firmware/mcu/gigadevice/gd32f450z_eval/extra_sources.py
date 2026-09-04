Import("env")
from pathlib import Path

project_dir = Path(env.subst("$PROJECT_DIR")).resolve()
repo_root = project_dir.parents[3]
core_dir = repo_root / "firmware" / "core_driver" / "ads1299"
packet_dir = repo_root / "firmware" / "common" / "data_packet"
reference_dir = repo_root / "firmware" / "mcu" / "common" / "reference_app"

for include_dir in (core_dir, packet_dir, reference_dir, project_dir / "port"):
    env.Append(CPPPATH=[str(include_dir)])

env.BuildSources("$BUILD_DIR/ads1299_core", str(core_dir), src_filter=["+<*.c>"])
env.BuildSources("$BUILD_DIR/ads1299_packet", str(packet_dir), src_filter=["+<ads1299_packet.c>"])
env.BuildSources(
    "$BUILD_DIR/ads1299_reference_app",
    str(reference_dir),
    src_filter=["+<ads1299_reference_app.c>", "+<generic_board_port.c>", "+<generic_main.c>"],
)
