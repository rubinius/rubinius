##
# Platform specific behavior for the File class.

module FFI::Platform::File
  SEPARATOR = '/'
  ALT_SEPARATOR = nil
  PATH_SEPARATOR = ':'

  # FIXME: this is awful
  def self.expand_path(path, dir_string = nil)
    path = StringValue(path)
    if(dir_string.nil?)
      dir_string = Dir.pwd
    else
      dir_string = StringValue(dir_string)
    end

    path.gsub!(/~(#{ENV['USER']})/, "~/")

    raise ArgumentError, "user #{path}" if path.match(/~([^\/])/)

    if(dir_string.empty?)
      dir_string = Dir.pwd
    elsif(dir_string[0].chr == '~')
      dir_string = ENV['HOME'] + dir_string[1..-1]
    elsif(dir_string[0].chr != '/')
      dir_string = Dir.pwd + "/" + dir_string
    end

    dirs = path.split('/')
    if path == '' || (dirs.empty? && path[0].chr != '/')
      return dir_string
    else
      first = case dirs.first
              when '..'; dir_string.split('/')[0...-1].join('/')
              when '~'; ENV['HOME']
              when '.'; dir_string
              when ''; '/'
              when nil;
                match = /(\/+)/.match(path)
                prefix = match[0] if match
                ''
              else
                dir_string + '/' + dirs.first
              end

      dirs.shift
      paths = first.split('/')
      dirs.each do |dir|
        next if dir == '.' || dir == ''
        dir == '..' ? paths.pop : paths.push(dir)
      end
      string = paths.empty? ? '' : paths.join("/")
      return !string.empty? && string[0].chr == '/' ? string : prefix || '/' +string
    end
  end
end
