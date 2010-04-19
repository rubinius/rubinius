require 'spec/custom/utils/script'

class MSpecScript
  # Language features specs
  set :language, [
    'spec/ruby/language',
    'spec/language'
  ]

  # Core library specs
  set :core, [
    'spec/ruby/core',
    'spec/core',

    # 1.9
    '^core/basicobject'
  ]

  # Standard library specs
  set :library, [
    'spec/ruby/library',
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
    '^spec/capi/module',
    '^spec/capi/proc',
    '^spec/capi/struct'
  ]

  set :command_line, [ 'spec/command_line' ]

  set :compiler, [ 'spec/compiler' ]

  set :build, [ 'spec/build' ]

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library) +
              get(:capi) + get(:compiler) + get(:build)

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
    'spec/ruby/core',
    'spec/ruby/language',
    'spec/core',
    'spec/compiler',
    'spec/command_line',
    'spec/capi',
    'spec/ruby/library/digest',

    'spec/build',

    '^spec/core/compiledmethod',
    '^spec/core/module',
    '^spec/capi/globals',
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
                        [%r(spec/), 'spec/tags/'],
                        [/_spec.rb$/, '_tags.txt']
                      ]

  # Prepended to file names when resolving spec files. Enables
  # commands like 'bin/mspec core/array' to be equivalent to
  # 'bin/mspec spec/ruby/core/array'
  set :prefix, 'spec/ruby'

  # The default implementation to run the specs.
  set :target, 'bin/rbx'

  # Leave out MSpec lines from backtraces
  set :backtrace_filter, %r[(mspec/bin|mspec/lib/mspec)]

  # Enable language features
  MSpec.enable_feature :fork

  if Object.const_defined?(:Rubinius) && Rubinius.const_get(:Fiber)
    ::Fiber = Rubinius::Fiber
    MSpec.enable_feature :fiber
  end
end
