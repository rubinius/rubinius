class Object
  # Creates each directory in path that does not exist.
  def mkdir_p(path)
    parts = File.expand_path(path).split %r[/|\\]
    name = parts.shift
    parts.each do |part|
      name = File.join name, part

      if File.file? name
        raise ArgumentError, "path component of #{path} is a file"
      end

      Dir.mkdir name unless File.directory? name
    end
  end

  # Recursively removes all files and directories in +path+
  # if +path+ is a directory. Removes the file if +path+ is
  # a file.
  def rm_r(path)
    path = File.expand_path path

    prefix = SPEC_TEMP_DIR
    unless path[0, prefix.size] == prefix
      raise ArgumentError, "#{path} is not prefixed by #{prefix}"
    end

    if File.file? path
      File.delete path
    elsif File.directory? path
      Dir.entries(path).each { |x| rm_r "#{path}/#{x}" unless x =~ /^\.\.?$/ }
      Dir.rmdir path
    end
  end

  # Creates a file +name+. Creates the directory for +name+
  # if it does not exist.
  def touch(name)
    mkdir_p File.dirname(name)

    File.open(name, "w") do |f|
      yield f if block_given?
    end
  end
end
