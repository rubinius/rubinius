# vim: filetype=ruby

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
    '^library/socket/addrinfo',
  ]

  set :capi, [
    '^spec/ruby/optional/capi/encoding',
  ]

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library) +
              get(:capi) + get(:compiler) + get(:build) + get(:command_line)

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
    '^spec/ruby/library/socket/addrinfo',
  ]

  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/ruby/core',
    'spec/ruby/language',
    'spec/ruby/command_line',
    'spec/core',
    'spec/language',
    'spec/compiler',
    'spec/command_line',
    'spec/ruby/optional/capi',
    'spec/ruby/library',
    'spec/library',

    'spec/build',

    # We use the FFI gem now
    '^spec/ruby/optional/ffi',

    # Excluded because the specs are extremely system dependent.
    '^spec/ruby/library/syslog',

    # Excluded because they fail to load at the moment
    '^spec/ruby/optional/ffi/buffer_spec.rb',
    '^spec/ruby/optional/ffi/callback_spec.rb',
    '^spec/ruby/optional/ffi/custom_type_spec.rb',
    '^spec/ruby/optional/ffi/ffi_spec.rb',
    '^spec/ruby/optional/ffi/function_spec.rb',
    '^spec/ruby/optional/ffi/number_spec.rb',
    '^spec/ruby/optional/ffi/pointer_spec.rb',
    '^spec/ruby/optional/ffi/strptr_spec.rb',
    '^spec/ruby/optional/ffi/struct_initialize_spec.rb',
    '^spec/ruby/optional/ffi/struct_spec.rb',
    '^spec/ruby/optional/ffi/variadic_spec.rb',

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
    '^spec/ruby/library/socket/addrinfo',
    '^spec/ruby/optional/capi/encoding',
  ]

  set :travis, get(:ci_files) - [
    'spec/library',
    'spec/ruby/library',
    'spec/ruby/optional/capi'
  ]

  # The set of substitutions to transform a spec filename
  # into a tag filename.
  set :tags_patterns, [
                        [%r(spec/), 'spec/tags/18/'],
                        [/_spec.rb$/, '_tags.txt']
                      ]

  # Enable language features
  if Rubinius::Config["hash.hamt"]
    MSpec.enable_feature :hash_hamt
  else
    MSpec.enable_feature :hash_bucket
  end

  # The Readline specs are not enabled by default because the functionality
  # depends heavily on the underlying library, including whether certain
  # methods are implemented or not. This makes it extremely difficult to
  # make the specs consistently pass. Until a suitable scheme to handle
  # all these issues, the specs will not be enabled by default.
  #
  # MSpec.enable_feature :readline
end
