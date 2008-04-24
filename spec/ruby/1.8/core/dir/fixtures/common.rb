require 'fileutils'

module DirSpecs
  def DirSpecs.mock_dir
    unless defined? @mock_dir then
      @mock_dir = File.expand_path(tmp('mock'))
    end

    @mock_dir
  end

  def DirSpecs.nonexistent
    name = mock_dir + "/nonexistent00"
    name = name.next while File.exist? name
    name
  end

  def DirSpecs.clear_dirs
    ['nonexisting', 'default_perms','reduced', 'always_returns_0'].each do |dir|
      begin
        Dir.rmdir dir
      rescue
      end
    end
  end

  def DirSpecs.create_mock_dirs
    mock_dir = self.mock_dir
    files = %W[ #{mock_dir}/.dotfile
                #{mock_dir}/.dotsubdir/.dotfile
                #{mock_dir}/.dotsubdir/nondotfile

                #{mock_dir}/deeply/.dotfile
                #{mock_dir}/deeply/nested/.dotfile.ext
                #{mock_dir}/deeply/nested/directory/structure/.ext
                #{mock_dir}/deeply/nested/directory/structure/bar
                #{mock_dir}/deeply/nested/directory/structure/baz
                #{mock_dir}/deeply/nested/directory/structure/file_one
                #{mock_dir}/deeply/nested/directory/structure/file_one.ext
                #{mock_dir}/deeply/nested/directory/structure/foo
                #{mock_dir}/deeply/nondotfile

                #{mock_dir}/file_one.ext
                #{mock_dir}/file_two.ext

                #{mock_dir}/nondotfile

                #{mock_dir}/subdir_one/.dotfile
                #{mock_dir}/subdir_one/nondotfile
                #{mock_dir}/subdir_two/nondotfile
                #{mock_dir}/subdir_two/nondotfile.ext

                #{mock_dir}/special/+

                #{mock_dir}/special/^
                #{mock_dir}/special/$

                #{mock_dir}/special/(
                #{mock_dir}/special/)
                #{mock_dir}/special/[
                #{mock_dir}/special/]
                #{mock_dir}/special/{
                #{mock_dir}/special/}
              ]

    platform_is_not :windows do
      files += %W[  #{mock_dir}/special/*
                    #{mock_dir}/special/?

                    #{mock_dir}/special/|
                  ]
    end
    
    FileUtils.rm_rf mock_dir
    files.each do |file|
      FileUtils.mkdir_p File.dirname(file)
      # eventually will be FileUtils.touch(File.basename(dir))
      File.open(file, "w") { }
    end
  end

  def self.expected_paths
    %w[
      .
      ..
      .dotfile
      .dotsubdir
      deeply
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
