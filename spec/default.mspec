require 'spec/custom/utils/script'

class MSpecScript
  # Language features specs
  set :language, [
    'spec/frozen/language',
    'spec/language'
  ]

  # Core library specs
  set :core, [
    'spec/frozen/core',
    'spec/core',

    # 1.9
    '^core/basicobject'
  ]

  # Standard library specs
  set :library, [
    'spec/frozen/library',
    'spec/library',

    # 1.9 features
    '^library/cmath',
    '^library/continuation',
    '^library/coverage',
    '^library/fiber',
    '^library/json',
    '^library/minitest',
    '^library/prime',
    '^library/ripper',
    '^library/rake',
    '^library/rubygems',
  ]

  set :capi, [
    'spec/capi',
    '^spec/capi/globals',
    '^spec/capi/language',
    '^spec/capi/module',
    '^spec/capi/proc',
    '^spec/capi/struct'
  ]

  set :compiler, [ 'spec/compiler' ]

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library) +
              get(:capi) + get(:compiler)

  set :ruby, [
    'spec/ruby/language',
    'spec/ruby/core',
    'spec/ruby/library',

    # 1.9 features
    '^spec/ruby/core/basicobject',
    '^spec/ruby/library/cmath',
    '^spec/ruby/library/continuation',
    '^spec/ruby/library/coverage',
    '^spec/ruby/library/fiber',
    '^spec/ruby/library/json',
    '^spec/ruby/library/minitest',
    '^spec/ruby/library/prime',
    '^spec/ruby/library/ripper',
    '^spec/ruby/library/rake',
    '^spec/ruby/library/rubygems',
  ]

  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/core',
    'spec/frozen/language',
    'spec/core',
    'spec/compiler',
    'spec/capi',
    'spec/frozen/library/digest',

    '^spec/core/compiledmethod',
    '^spec/core/module',
    '^spec/capi/globals',
    '^spec/capi/language',
    '^spec/capi/module',
    '^spec/capi/proc',
    '^spec/capi/struct',

    # These additional directories will be enabled as the
    # specs in them are updated for the C++ VM.
    # 'spec/debugger',
  ]

  # The set of substitutions to transform a spec filename
  # into a tag filename.
  set :tags_patterns, [
                        [%r(spec/ruby/), 'spec/frozen/'],
                        [%r(spec/), 'spec/tags/'],
                        [/_spec.rb$/, '_tags.txt']
                      ]

  # Prepended to file names when resolving spec files. Enables
  # commands like 'bin/mspec core/array' to be equivalent to
  # 'bin/mspec spec/frozen/core/array'
  set :prefix, 'spec/frozen'

  # The default implementation to run the specs.
  set :target, 'bin/rbx'

  # Leave out MSpec lines from backtraces
  set :backtrace_filter, %r[(mspec/bin|mspec/lib/mspec)]
end
