class MSpecScript
  # Language features specs
  set :language, [ 'language' ]

  # Core library specs
  set :core, [ 'core' ]

  # Standard library specs
  set :library, [
    'library',

    # obsolete libraries
    '^library/cgi-lib',
    '^library/date2',
    '^library/eregex',
    '^library/finalize',
    '^library/ftools',
    '^library/generator',
    '^library/getopts',
    '^library/importenv',
    '^library/jcode',
    '^library/mailread',
    '^library/parsearg',
    '^library/parsedate',
    '^library/ping',
    '^library/readbytes',
    '^library/rubyunit',
    '^library/runit',
    '^library/soap',
    '^library/wsdl',
    '^library/xsd',
    '^library/Win32API',

    '^library/test/unit/collector',
    '^library/test/unit/ui',
    '^library/test/unit/util',

    '^library/dl',  # reimplemented and API changed
  ]

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library)

  # This set of files is run by mspec ci
  set :ci_files, get(:files)

  # The default implementation to run the specs.
  # TODO: this needs to be more sophisticated since the
  # executable is not consistently named.
  set :target, 'ruby1.9'

  set :tags_patterns, [
                        [%r(language/), 'tags/1.9/language/'],
                        [%r(core/),     'tags/1.9/core/'],
                        [%r(library/),  'tags/1.9/library/'],
                        [/_spec.rb$/,   '_tags.txt']
                      ]
end
