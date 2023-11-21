project('pong', 'c',
  version : '1.0',
  default_options : ['warning_level=3'])

cc = meson.get_compiler('c')
cmath = cc.find_library('m', required : false)

sdl2 = dependency('SDL2', version : '2.26.5')
sdl2_ttf = dependency('SDL2_ttf', version : '2.20.2')
logc = dependency('log.c', version : 'cci.20200620')
cloveunit = dependency('clove-unit', version : '2.4.1')

exe = executable('pong',
                 'src/main.c',
                 'src/game.c',
                 'src/graphics.c',
                 'src/physics.c',
                 install : false,
                 dependencies : [sdl2, sdl2_ttf, logc, cloveunit, cmath],
                )
