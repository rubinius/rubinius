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

  def self.basename(path,ext)
    basename = if(m = path.match(/#{SEPARATOR}+([^#{SEPARATOR}]*)#{SEPARATOR}*$/))
                 m[1] == '' ? SEPARATOR : m[1]
               else
                 path
               end
    ext = Regexp.quote(ext) # convert everything to literal characters
    ext.sub!(/\\\*/,'[^\.]+?') # convert .*'s back into a general match expression
    return basename.sub(/#{ext}$/,'')
  end
end
