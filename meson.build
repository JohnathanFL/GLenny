project('Glenny', 'cpp',
  default_options : ['cpp_std=c++17'])
cpp = meson.get_compiler('cpp')

srcs = [
  'src/main.cpp',
  'src/App.cpp',
  'src/Engine.cpp'
]

include = include_directories('include')

dl = cpp.find_library('dl', required: true)
gl = dependency('GL')

sdl2 = dependency('sdl2')


magnum = cpp.find_library('Magnum', required: true)
magnumCtx = cpp.find_library('MagnumGlxContext', required: true)
magnumGL = cpp.find_library('MagnumGL', required: true)
magnumDeps = [magnum, magnumGL, magnumCtx]


executable('glenny', srcs, dependencies: [gl, dl, sdl2] + magnumDeps, include_directories: include)

