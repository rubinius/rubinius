class MSpecScript
  # An ordered list of the directories containing specs to run
  set :files, [
    'language',
    'core',
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

  # The default implementation to run the specs.
  set :target, 'ruby'
end
