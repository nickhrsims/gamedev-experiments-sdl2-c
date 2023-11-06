project('pong', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

cc = meson.get_compiler('c')
cmath = cc.find_library('m', required : false)

sdl2 = dependency('sdl2', version : '2.28.3')
logc = dependency('log.c', version : 'cci.20200620')
cloveunit = dependency('clove-unit', version : '2.4.1')

exe = executable('pong',
                 'main.c',
                 install : false,
                 dependencies : [sdl2, logc, cloveunit, cmath],
                )
