require 'fileutils'

module DirSpecs
  def DirSpecs.mock_dir
    @mock_dir ||= Dir.chdir(File.dirname(__FILE__) + '/mock') { Dir.pwd }
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
    stem = File.dirname(__FILE__)
    files = %W[ #{stem}/mock/.dotfile
                #{stem}/mock/.dotsubdir/.dotfile
                #{stem}/mock/.dotsubdir/nondotfile

                #{stem}/mock/deeply/.dotfile
                #{stem}/mock/deeply/nested/.dotfile.ext
                #{stem}/mock/deeply/nested/directory/structure/.ext
                #{stem}/mock/deeply/nested/directory/structure/bar
                #{stem}/mock/deeply/nested/directory/structure/baz
                #{stem}/mock/deeply/nested/directory/structure/file_one
                #{stem}/mock/deeply/nested/directory/structure/file_one.ext
                #{stem}/mock/deeply/nested/directory/structure/foo
                #{stem}/mock/deeply/nondotfile

                #{stem}/mock/file_one.ext
                #{stem}/mock/file_two.ext

                #{stem}/mock/nondotfile

                #{stem}/mock/subdir_one/.dotfile
                #{stem}/mock/subdir_one/nondotfile
                #{stem}/mock/subdir_two/nondotfile
                #{stem}/mock/subdir_two/nondotfile.ext

                #{stem}/mock/special/+
                #{stem}/mock/special/*
                #{stem}/mock/special/?

                #{stem}/mock/special/|

                #{stem}/mock/special/^
                #{stem}/mock/special/$

                #{stem}/mock/special/(
                #{stem}/mock/special/)
                #{stem}/mock/special/[
                #{stem}/mock/special/]
                #{stem}/mock/special/{
                #{stem}/mock/special/}
              ]

    FileUtils.rm_rf "#{stem}/mock"
    files.each do |file|
      FileUtils.mkdir_p File.dirname(file)
      # eventually will be FileUtils.touch(File.basename(dir))
      File.open(file, "w") { }
    end
  end
end

# Create the fixture directories every time the specs are run
DirSpecs.create_mock_dirs
