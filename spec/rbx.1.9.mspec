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

    # Excluded while building out basic 1.9 infrastructure
    '^spec/ruby/core/encoding/default_internal',
    '^spec/ruby/core/io/open',
    '^spec/ruby/core/module/name',
    '^spec/ruby/core/numeric/to_c',
    '^spec/ruby/core/proc/arity',
    '^spec/ruby/core/process/spawn',
    '^spec/ruby/core/process/status',
    '^spec/ruby/core/string/getbyte',

    '^spec/ruby/library/csv/generate',
    '^spec/ruby/library/delegate',
    '^spec/ruby/library/logger',
    '^spec/ruby/library/net',
    '^spec/ruby/library/uri',
    '^spec/ruby/library/yaml',
    '^spec/ruby/library/zlib',

    '^spec/ruby/optional/capi/class',
    '^spec/ruby/optional/capi/io',
    '^spec/ruby/optional/capi/time',

    # Excluded because the specs are extremely system dependent.
    '^spec/ruby/library/syslog',
  ] + get(:obsolete_library)

  # The set of substitutions to transform a spec filename
  # into a tag filename.
  set :tags_patterns, [
                        [%r(spec/), 'spec/tags/19/'],
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
