### ----------------------------------------------------------------------------
### Meson Configuration
### ----------------------------------------------------------------------------

project('pong', 'c',
  version : '1.0',
  default_options : [
    'warning_level=3',
    'b_ndebug=if-release',
    'buildtype=debug',
  ]
)

### ----------------------------------------------------------------------------
### Compiler Configuration
### ----------------------------------------------------------------------------

cc = meson.get_compiler('c')
cmath = cc.find_library('m', required : false)

### ----------------------------------------------------------------------------
### Dependencies
### ----------------------------------------------------------------------------

sdl2 = dependency('SDL2', version : '2.26.5')
sdl2_ttf = dependency('SDL2_ttf', version : '2.20.2')
logc = dependency('log.c', version : 'cci.20200620')
cloveunit = dependency('clove-unit', version : '2.4.1')

### ----------------------------------------------------------------------------
### Primary Build Target
### ----------------------------------------------------------------------------

exe = executable('pong',
                 'src/app/app.c',
                 'src/app/video.c',
                 'src/game/actions.c',
                 'src/game/collision.c',
                 'src/game/game.c',
                 'src/game/field.c',
                 'src/game/player.c',
                 'src/game/ball.c',
                 'src/game/entity.c',
                 'src/game/paddle.c',
                 'src/fsm/fsm.c',
                 'src/aabb.c',
                 'src/main.c',
                 install : false,
                 include_directories : ['src'],
                 dependencies : [ sdl2, sdl2_ttf, logc, cloveunit, cmath ],
                 )

### ----------------------------------------------------------------------------
### Tests 
### ----------------------------------------------------------------------------

### ------------------------------------
### FSM Tests
### ------------------------------------

test('FSM / Smoke Test',
  executable('test-fsm-smoke',
             'src/fsm/fsm.c',
             'src/fsm/test/smoke.c',
             install : false,
             include_directories : ['src'],
             dependencies : [],
  )
)
