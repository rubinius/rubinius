# just for compatibility; requiring "md5" is obsoleted

require 'digest/md5'

class MD5 < Digest::MD5
  class << self
    alias orig_new new

    def new(str=nil)
      if str
        orig_new.update(str)
      else
        orig_new
      end
    end

    def md5(*args)
      new(*args)
    end
  end
end
