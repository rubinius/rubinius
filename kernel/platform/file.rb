# depends on: ffi.rb

module Platform::File
  SEPARATOR = '/'
  ALT_SEPARATOR = nil

  def self.dirname(path)
    if(!path.match(/#{SEPARATOR}/) || path.match(/^\.+#{SEPARATOR}+$/))
      return "."
    else
      # strip the basename off the end and clean up the ending separators
      path.sub!(/#{SEPARATOR}*[^#{SEPARATOR}]*#{SEPARATOR}*$/,'')    
      if(path == '')
        return SEPARATOR
      else
        return path
      end
    end
  end  
end
