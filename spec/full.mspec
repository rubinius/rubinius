load File.dirname(__FILE__) + '/default.mspec'

class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/1.8/core',
    'spec/frozen/1.8/language',
    'spec/compiler',
    '^spec/compiler/new_compiler_spec.rb',
    'spec/core',
    'spec/frozen/1.8/library',
    '^spec/frozen/1.8/library/iconv',
    '^spec/frozen/1.8/library/readline',
    'spec/library',
  ]
end
