def mock_dir
  @mock_dir ||= Dir.chdir(File.dirname(__FILE__) + '/mock') { Dir.pwd }
end

def nonexistent
  name = mock_dir + "/nonexistent00"
  name = name.next while File.exist? name
  name
end
