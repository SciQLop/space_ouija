catch2_dep = dependency('catch2-with-main', version:'>3.0.0', required : true)

resources_dir = meson.current_source_dir() + '/resources'

tests = [
    {
        'name':'Cassini_RPWS_LOW_RATE_FULL_MFR0',
        'sources': ['Cassini/test_RPWS_LOW_RATE_FULL_MFR0.cpp'],
        'deps': [space_ouija_dep, catch2_dep]
    },
    {
        'name':'Cassini_RPWS_WBR_WFR',
        'sources': ['Cassini/test_RPWS_WBR_WFR.cpp'],
        'deps': [space_ouija_dep, catch2_dep]
    },
    {
      'name':'Cassini_manal_test',
      'sources': ['Cassini/manual_test.cpp'],
      'deps': [space_ouija_dep]
    },
]

foreach unit_test : tests
  test_exe = executable(unit_test['name'],unit_test['sources'],
                          dependencies : unit_test['deps'],
                          cpp_args : ['-DRESOURCES_DIR="@0@"'.format(resources_dir)]
						  )
  test('Test-' + unit_test['name'], test_exe, args:['-o','@0@_out.xml'.format(unit_test['name']), '-r', 'xml'])
endforeach
