class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/1.8/core',
    'spec/frozen/1.8/language',
    'spec/core',

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

  # The default implementation to run the specs.
  set :target, 'bin/rbx'
end
