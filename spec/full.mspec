load File.dirname(__FILE__) + '/default.mspec'

class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  set :ci_files, [
    'spec/frozen/1.8/core',
    'spec/frozen/1.8/language',
    'spec/compiler',
    'spec/core',
    'spec/debugger',
    'spec/subtend',
    'spec/frozen/1.8/library',
    'spec/library',
    # parser specs cause odd problems with library
    # specs, so they go last
    'spec/parser',
  ]
end
