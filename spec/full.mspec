load 'spec/default.mspec'

class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/core',
    'spec/frozen/language',
    'spec/core',
    'spec/compiler',
    'spec/command_line',
    'spec/capi',
    'spec/frozen/library',
    'spec/library',

    'spec/build',

    # excluded because significantly broken
    '^spec/core/compiledmethod',
    '^spec/core/module',
    '^spec/capi/globals',
    '^spec/capi/module',
    '^spec/capi/proc',
    '^spec/capi/struct',

    '^spec/frozen/library/net/ftp',
    '^spec/frozen/library/net/http',
    '^spec/frozen/library/ping',
    '^spec/frozen/library/syslog',

    # 1.9 features
    '^spec/frozen/library/cmath',
    '^spec/frozen/library/continuation',
    '^spec/frozen/library/coverage',
    '^spec/frozen/library/fiber',
    '^spec/frozen/library/json',
    '^spec/frozen/library/minitest',
    '^spec/frozen/library/prime',
    '^spec/frozen/library/ripper',
    '^spec/frozen/library/rake',
    '^spec/frozen/library/rubygems',
  ]
end
