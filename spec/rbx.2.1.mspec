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
  ]

  set :obsolete_library, [
    # obsolete libraries
    '^library/cgi-lib',
    '^library/date2',
    '^library/enumerator',
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

  # Standard library specs
  set :library, [
    'spec/ruby/library',
    'spec/library',
  ] + get(:obsolete_library)

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library) +
              get(:capi) + get(:compiler) + get(:build) + get(:command_line)

  set :ruby, [
    'spec/ruby/language',
    'spec/ruby/core',
    'spec/ruby/library',
  ] + get(:obsolete_library)

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
  ] + get(:obsolete_library)

  set :travis, get(:ci_files) - [
    'spec/library',
    'spec/ruby/library',
    'spec/ruby/optional/capi'
  ]

  # The set of substitutions to transform a spec filename
  # into a tag filename.
  set :tags_patterns, [
                        [%r(spec/), 'spec/tags/20/'],
                        [/_spec.rb$/, '_tags.txt']
                      ]

  # Enable language features
  MSpec.enable_feature :encoding

  # These are encoding-aware methods backported to 1.8.7+ (eg String#bytes)
  MSpec.enable_feature :encoding_transition

  if Rubinius::Config["hash.hamt"]
    MSpec.enable_feature :hash_hamt
  else
    MSpec.enable_feature :hash_bucket
  end

  module ::Kernel
    alias_method :require, :gem_original_require
  end
end
