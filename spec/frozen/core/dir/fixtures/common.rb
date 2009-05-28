require 'fileutils'

module DirSpecs
  def DirSpecs.mock_dir(dirs = ['mock'])
    File.expand_path(tmp(File.join(dirs)))
  end

  def DirSpecs.nonexistent
    name = mock_dir + "/nonexistent00"
    name = name.next while File.exist? name
    name
  end

  def DirSpecs.clear_dirs
    old_kcode, $KCODE = $KCODE, 'u'
    ['nonexisting', 'default_perms','reduced', 'always_returns_0', '???', [0xe9].pack('U')].each do |dir|
      begin
        Dir.rmdir dir
      rescue
      end
    end
  end

  def DirSpecs.create_mock_dirs
    mock_dir = self.mock_dir
    files = %w[
      .dotfile
      .dotsubdir/.dotfile
      .dotsubdir/nondotfile

      deeply/.dotfile
      deeply/nested/.dotfile.ext
      deeply/nested/directory/structure/.ext
      deeply/nested/directory/structure/bar
      deeply/nested/directory/structure/baz
      deeply/nested/directory/structure/file_one
      deeply/nested/directory/structure/file_one.ext
      deeply/nested/directory/structure/foo
      deeply/nondotfile

      file_one.ext
      file_two.ext

      dir_filename_ordering
      dir/filename_ordering

      nondotfile

      subdir_one/.dotfile
      subdir_one/nondotfile
      subdir_two/nondotfile
      subdir_two/nondotfile.ext

      special/+

      special/^
      special/$

      special/(
      special/)
      special/[
      special/]
      special/{
      special/}
    ]

    platform_is_not :windows do
      files += %w[
        special/*
        special/?

        special/|
      ]
    end
    
    umask = File.umask 0
    FileUtils.rm_rf mock_dir
    files.each do |file|
      file = File.join mock_dir, file
      FileUtils.mkdir_p File.dirname(file)
      FileUtils.touch file
    end
    File.umask umask
  end

  def self.mock_rmdir(*dirs)
    mock_dir ['rmdir_dirs'].concat(dirs)
  end

  def self.rmdir_dirs(create = true)
    dirs = %w[
      empty
      nonempty
      nonempty/child
      noperm
      noperm/child
    ]

    base_dir = mock_dir ['rmdir_dirs']

    dirs.reverse_each do |d|
      dir = File.join base_dir, d
      if File.exists? dir
        File.chmod 0777, dir
        FileUtils.rm_rf dir
      end
    end
    FileUtils.rm_rf base_dir

    if create
      dirs.each do |d|
        dir = File.join base_dir, d
        unless File.exists? dir
          FileUtils.mkdir_p dir
          File.chmod 0777, dir
        end
      end
    end
  end

  def self.expected_paths
    %w[
      .
      ..
      .dotfile
      .dotsubdir
      deeply
      dir
      dir_filename_ordering
      file_one.ext
      file_two.ext
      nondotfile
      special
      subdir_one
      subdir_two
    ]
  end

end

# Create the fixture directories every time the specs are run
DirSpecs.create_mock_dirs
