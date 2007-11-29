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
end
