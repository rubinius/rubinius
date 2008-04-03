class MSpecScript
  # An ordered list of the directories containing specs to run
  # as the CI process.
  Config[:ci_files] = [
    'spec/ruby/1.8/core',
    'spec/ruby/1.8/language',
    'spec/compiler',
    'spec/core',
    'spec/debugger',
    'spec/subtend',
    'spec/ruby/1.8/library',
    # parser specs cause odd problems with library
    # specs, so they go last
    'spec/parser',
  ]

  # The directory to search for tags for each spec file
  # processed. The directory is relative to the directory
  # in which the spec runner is invoked, in this case,
  # the rubinius root.
  Config[:tags_dir] = File.expand_path('spec/tags')

  # The default implementation to run the specs.
  Config[:target] = 'shotgun/rubinius'
end
