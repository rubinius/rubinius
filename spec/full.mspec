load 'spec/default.mspec'

class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/core',
    'spec/frozen/language',
    'spec/compiler',
    'spec/core',
    'spec/frozen/library',
    'spec/library',

    # excluded because significantly broken
    '^spec/frozen/library/net/ftp',
    '^spec/frozen/library/net/http',
    '^spec/frozen/library/ping',
    '^spec/frozen/library/readline',
    '^spec/frozen/library/syslog',

    # excluded as 1.9 libraries
    '^spec/frozen/library/prime',
  ]
end
