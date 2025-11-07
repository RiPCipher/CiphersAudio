#!/usr/bin/env python
import os

env = SConscript("godot-cpp/SConstruct")

# Add include paths
env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=["thirdparty/pffft/"])

# Windows SIMD flags
if env["platform"] == "windows":
    if env.get("use_mingw", False):
        env.Append(CCFLAGS=["-msse2", "-mfpmath=sse"])
    else:
        env.Append(CCFLAGS=["/arch:SSE2"])

env.Append(CPPDEFINES=["PFFFT_ENABLE_FLOAT"])

# Extension source files
sources = Glob("src/*.cpp")

# pffft sources
pffft_sources = [
    "thirdparty/pffft/pffft.c",
    "thirdparty/pffft/pffft_common.c",
]

# Compile pffft with warnings disabled
env_pffft = env.Clone()
if env["platform"] == "windows" and not env.get("use_mingw", False):
    env_pffft.Append(CXXFLAGS=["/TP"])  # Compile as C++
    env_pffft.Append(CCFLAGS=["/wd4244", "/wd4305", "/wd4204", "/wd4456"])
else:
    env_pffft.Append(CCFLAGS=["-w"])

pffft_objects = []
for src in pffft_sources:
    if env["platform"] == "windows" and not env.get("use_mingw", False):
        pffft_objects.append(env_pffft.SharedObject(src))
    else:
        pffft_objects.append(env_pffft.Object(src))

sources += pffft_objects

# Build
library = env.SharedLibrary(
    "bin/ciphersaudio{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

Default(library)
