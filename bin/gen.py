#!/usr/bin/python
import glob
import ntpath
import os
import sys

EXIT_SUCCESS = 0
EXIT_FAILURE = 1


def prepare_deps(deps, fname):
    lines = []
    basename = ntpath.basename(fname)
    name = basename.replace('.cpp', '')
    fname_h = fname.replace('.cpp', '.h')
    ref = fname
    if os.path.isfile(fname_h):
        ref += f' {fname_h}'
    lines.append(f'$(BPATH)/{name}$(EXT): {ref}')
    lines.append(f'\t$(CC) $(ARGS) -c $< $(INC) -o $@')
    deps.append(f'{name}')
    return '\n'.join(lines)


def get_deps_blocks():
    paths = ['src/*.cpp', "src/**/*.cpp"]
    deps_blocks = ["all: $(TARGET)"]
    deps = []

    for pattern in paths:
        for f in glob.glob(pattern):
            if ntpath.basename(f).startswith("_"):
                continue
            deps_blocks.append(prepare_deps(deps, f))

    objs = ' '.join(f'$(BPATH)/{x}$(EXT)' for x in deps)
    lines = []
    lines.append(f'$(TARGET): $(DEPS)')
    lines.append(f'\t$(CC) $(ARGS) $(DEPS) $(LIBS) $(PARGS) -o $@')
    deps_blocks.append('\n'.join(lines))
    lines = []
    lines.append('clean:')
    lines.append("\trm -f $(BPATH)/*")
    lines.append('run:')
    lines.append('\t$(RUN)')

    deps_blocks.append('\n'.join(lines))
    return deps_blocks, objs


help_text = '''
makefile generator

must have only one arg.
possible values are sdl and emsdl

example:

python bin/gen.py sdl
python bin/gen.py emsdl

'''.strip()


def main():
    if len(sys.argv) != 2 or sys.argv[1] not in ['sdl', 'emsdl']:
        print(help_text)
        exit(EXIT_FAILURE)
    elif sys.argv[1] == 'sdl':
        vars = [
            'CC=g++',
            'INC=',
            'LIBS=-lSDL2 -lz',
            'ARGS=-O3',
            'PARGS=',
            'BPATH=build', 'BNAME=jewels-runtime',
            'TARGET=$(BPATH)/$(BNAME)',
            'RUN=$(TARGET)'
        ]
        print("type `make` to generare binary.")
        ext = '.o'
    elif sys.argv[1] == 'emsdl':
        vars = [
            'CC=emcc',
            'INC=',
            'LIBS=',
            'ARGS=-s USE_SDL=2 -s USE_ZLIB=1 -DWASM',
            'PARGS=--preload-file data --emrun -O2 -s WASM=1',
            'BPATH=build', 'BNAME=jewels.html',
            'TARGET=$(BPATH)/$(BNAME)',
            'RUN=emrun $(TARGET)'
        ]
        print("type `emmake make` to generare binary.")
        ext = '.o'

    deps_blocks, objs = get_deps_blocks()
    vars.append(f'DEPS={objs}')
    vars.append(f'EXT={ext}')
    with open('Makefile', 'w') as tfile:
        tfile.write('\n'.join(vars) + "\n\n")
        tfile.write('\n\n'.join(deps_blocks))

    if not os.path.exists('build/'):
        os.makedirs('build/')
    return EXIT_SUCCESS


exit(main())
