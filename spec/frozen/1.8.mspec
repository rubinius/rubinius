class MSpecScript
  # An ordered list of the directories containing specs to run
  set :files, [
    '1.8/language',
    '1.8/core',
    '1.8/library'
  ]

  # The default implementation to run the specs.
  set :target, 'ruby'
end
