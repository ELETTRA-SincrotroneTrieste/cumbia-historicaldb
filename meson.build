project('cumbia-hdb', 'cpp', version : '1.0.0',
    default_options : ['c_std=c17', 'cpp_std=c++17'])

project_description = 'use cumbia to extract data from hdb and hdb++ historical databases'

cu_hdb_version = meson.project_version() # set in project() below
ver_arr = cu_hdb_version.split('.')

cu_hdb_major_version = ver_arr[0]
cu_hdb_minor_version = ver_arr[1]
cu_hdb_micro_version = ver_arr[2]

conf = configuration_data()
# Surround the version in quotes to make it a C string
conf.set_quoted('VERSION', cu_hdb_version)
configure_file(output : 'config.h',
               configuration : conf)


headers = [
        'src/cuhdbactionfactories.h',
        'src/cuhdbactionfactoryi.h',
    'src/cuhdbactionfactoryservice.h',
    'src/cuhdbactioni.h',
    'src/cuhdbactionreader.h',
    'src/cuhdbfetchactivity.h',
    'src/cumbiahdb.h',
    'src/cumbiahdbworld.h',
    'src/hdb_source.h'
]

install_headers(headers, subdir : 'cumbia-hdb') # -> include/cumbia/

sources = [
'src/cuhdbactionfactories.cpp',
'src/cuhdbactionfactoryservice.cpp',
'src/cuhdbactioni.cpp',
'src/cuhdbactionreader.cpp',
'src/cuhdbfetchactivity.cpp',
'src/cumbiahdb.cpp',
'src/cumbiahdbworld.cpp',
'src/hdb_source.cpp'

]

includedirs = include_directories('src')

cpp_arguments = [  '-DCUMBIA_DEBUG_OUTPUT=1' ]

deps = [ dependency('threads'), dependency('cumbia'), dependency('hdbextractor++') ]

cumbialib = shared_library('cumbia-hdb', sources,
        version : meson.project_version(),
        include_directories : includedirs,
        cpp_args : cpp_arguments,
        dependencies : deps ,
        install : true)

### ====================================== pkg config   ============================
pkgconfig = find_program('pkg-config', required: false)
if not pkgconfig.found()
  error('MESON_SKIP_TEST: pkg-config not found')
endif

pkgg = import('pkgconfig')

h = ['cumbia-hdb']  # subdirectories of ${prefix}/${includedir} to add to header path
pkgg.generate(
    libraries : cumbialib,
    subdirs : h,
    version : meson.project_version(),
    name : 'libcumbia-hdb',
    filebase : 'cumbia-hdb',
    requires: ['hdbextractor++', 'cumbia'],
    description : project_description )


### ====================================== documentation ===========================
doxygen = find_program('doxygen', required : false)
if not doxygen.found()
  error('MESON_SKIP_TEST doxygen not found.')
endif
  doxydata = configuration_data()
  doxydata.set('VERSION', meson.project_version())


if find_program('dot', required : false).found()
  # In the real world this would set the variable
  # to YES. However we set it to NO so that the
  # list of generated files is always the same
  # so tests always pass.
  doxydata.set('HAVE_DOT', 'YES')
else
  doxydata.set('HAVE_DOT', 'NO')
endif

subdir('doc')

