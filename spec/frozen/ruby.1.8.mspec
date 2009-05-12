class MSpecScript
  # Language features specs
  set :language, [ 'language' ]

  # Core library specs
  set :core, [
    'core',

    # 1.9
    '^core/basicobject'
  ]

  # Standard library specs
  set :library, [
    'library',

    # 1.9 feature
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

  # An ordered list of the directories containing specs to run
  set :files, get(:language) + get(:core) + get(:library)

  # The default implementation to run the specs.
  set :target, 'ruby'
end
