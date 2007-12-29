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
    files = %w[ spec/core/dir/fixtures/mock/.dotfile
                spec/core/dir/fixtures/mock/.dotsubdir/.dotfile
                spec/core/dir/fixtures/mock/.dotsubdir/nondotfile
                spec/core/dir/fixtures/mock/deeply/.dotfile
                spec/core/dir/fixtures/mock/deeply/nested/.dotfile.ext
                spec/core/dir/fixtures/mock/deeply/nested/directory/structure/.ext
                spec/core/dir/fixtures/mock/deeply/nested/directory/structure/bar
                spec/core/dir/fixtures/mock/deeply/nested/directory/structure/baz
                spec/core/dir/fixtures/mock/deeply/nested/directory/structure/file_one
                spec/core/dir/fixtures/mock/deeply/nested/directory/structure/file_one.ext
                spec/core/dir/fixtures/mock/deeply/nested/directory/structure/foo
                spec/core/dir/fixtures/mock/deeply/nondotfile
                spec/core/dir/fixtures/mock/file_one.ext
                spec/core/dir/fixtures/mock/file_two.ext
                spec/core/dir/fixtures/mock/nondotfile
                spec/core/dir/fixtures/mock/subdir_one/.dotfile
                spec/core/dir/fixtures/mock/subdir_one/nondotfile
                spec/core/dir/fixtures/mock/subdir_two/nondotfile
                spec/core/dir/fixtures/mock/subdir_two/nondotfile.ext]

    FileUtils.rm_rf 'spec/core/dir/fixtures/mock/'
    files.each do |file|
      FileUtils.mkdir_p File.dirname(file)
      # eventually will be FileUtils.touch(File.basename(dir))
      File.open(file, "w") { }
    end
  end
end

# Create the fixture directories every time the specs are run
DirSpecs.create_mock_dirs
