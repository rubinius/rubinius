require 'spec/custom/utils/script'

class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/core',
    'spec/frozen/language',
    'spec/core',
    'spec/compiler',

    '^spec/frozen/core/array/union_spec.rb',
    '^spec/frozen/core/array/uniq_spec.rb',
    '^spec/frozen/core/array/unshift_spec.rb',
    '^spec/frozen/core/array/values_at_spec.rb',
    '^spec/frozen/core/continuation',
    '^spec/frozen/core/process',
    '^spec/frozen/core/thread',
    '^spec/frozen/core/threadgroup',

    '^spec/frozen/language/hash_spec.rb',
    '^spec/frozen/language/numbers_spec.rb',

    '^spec/core/bignum/coerce_spec.rb',
    '^spec/core/compiledmethod',
    '^spec/core/module',

    '^spec/compiler/defn_spec.rb',
    '^spec/compiler/dstr_spec.rb',
    '^spec/compiler/lit_spec.rb',

    # These additional directories will be enabled as the
    # specs in them are updated for the C++ VM.
    # 'spec/compiler',
    # 'spec/debugger',
    # 'spec/subtend',
    # 'spec/parser',
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
end
