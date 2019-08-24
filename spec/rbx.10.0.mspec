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

  # Standard library specs
  set :library, [
    'spec/library',
  ]

  # JIT specs
  set :jit, [
    'spec/jit',
  ]

  set :instructions, [
    'spec/instructions',
  ]

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library) + get(:instructions) + get(:jit) +
              get(:capi) + get(:compiler) + get(:build) + get(:command_line)

  set :ruby, [
    'spec/ruby',
  ]

  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/ruby/core',
    'spec/ruby/language',
    'spec/ruby/command_line',
    'spec/core',
    'spec/instructions',
    'spec/jit',
    'spec/language',
    'spec/command_line',
    'spec/ruby/optional/capi',
    'spec/library',
  ]

  set :travis, get(:ci_files)

  # The set of substitutions to transform a spec filename
  # into a tag filename.
  set :tags_patterns, [
                        [%r(spec/), 'spec/tags/'],
                        [/_spec.rb$/, '_tags.txt']
                      ]

  # Enable language features
  MSpec.enable_feature :encoding

  # These are encoding-aware methods backported to 1.8.7+ (eg String#bytes)
  MSpec.enable_feature :encoding_transition

  if Rubinius::JIT.enabled?
    MSpec.enable_feature :jit
  end
end
