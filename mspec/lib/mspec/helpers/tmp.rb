# Creates a temporary directory in the current working directory
# for temporary files created while running the specs. All specs
# should clean up any temporary files created so that the temp
# directory is empty when the process exits.

SPEC_TEMP_DIR = "#{File.expand_path(Dir.pwd)}/rubyspec_temp"

at_exit do
  begin
    Dir.delete SPEC_TEMP_DIR if File.directory? SPEC_TEMP_DIR
  rescue SystemCallError
    STDERR.puts <<-EOM

-----------------------------------------------------
The rubyspec temp directory is not empty. Ensure that
all specs are cleaning up temporary files.
-----------------------------------------------------

    EOM
  rescue Object => e
    STDERR.puts "failed to remove spec temp directory"
    STDERR.puts e.message
  end
end

class Object
  def tmp(name)
    Dir.mkdir SPEC_TEMP_DIR unless File.exists? SPEC_TEMP_DIR

    File.join SPEC_TEMP_DIR, name
  end
end
