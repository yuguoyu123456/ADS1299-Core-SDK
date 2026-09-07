"""Avoid cmd.exe's 8191-character limit for PlatformIO's IDF ldgen invocation.

No vendor files modified. SCons already supplies escaped Windows command tokens;
pass that command line directly to CreateProcess via subprocess, without a shell.
All other build commands retain the default PlatformIO spawn implementation.
"""
Import("env")
import os
import subprocess
if os.name == "nt":
    original_spawn = env["SPAWN"]
    def spawn(sh, escape, cmd, args, process_env):
        if any("ldgen.py" in arg for arg in args):
            return subprocess.call(" ".join(args), shell=False,
                                   env={str(k): str(v) for k, v in process_env.items()})
        return original_spawn(sh, escape, cmd, args, process_env)
    env["SPAWN"] = spawn
