project('cbuffer', 'c')

cmocka_dep = dependency('cmocka')
incdir = include_directories('inc')
sources = ['src/cbuffer.c', 'test/test-cbuffer.c']

e = executable('cbuffer', sources, include_directories: incdir, dependencies: cmocka_dep)
test('test/cbuffer_test', e)
