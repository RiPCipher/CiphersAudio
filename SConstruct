import os

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=["src/fft/"])
env.Append(CPPPATH=["src/generators/"])
env.Append(CPPPATH=["thirdparty/pffft/"])

if env["platform"] == "windows":
    if env.get("use_mingw", False):
        env.Append(CCFLAGS=["-msse2", "-mfpmath=sse"])
    else:
        env.Append(CCFLAGS=["/arch:SSE2"])

env.Append(CPPDEFINES=["PFFFT_ENABLE_FLOAT"])

sources = Glob("src/*.cpp")

fft_sources = Glob("src/fft/*.cpp")
sources += fft_sources

generator_sources = Glob("src/generators/*.cpp")
sources += generator_sources

pffft_sources = [
    "thirdparty/pffft/pffft.c",
    "thirdparty/pffft/pffft_common.c",
]

env_pffft = env.Clone()
if env["platform"] == "windows" and not env.get("use_mingw", False):
    env_pffft.Append(CXXFLAGS=["/TP"])
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

library = env.SharedLibrary(
    "bin/ciphersaudio{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

Default(library)
