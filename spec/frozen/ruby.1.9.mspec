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

  # The default implementation to run the specs.
  # TODO: this needs to be more sophisticated since the
  # executable is not consistently named.
  set :target, 'ruby19'
end
