# just for compatibility; requiring "md5" is obsoleted
#
# $RoughId: md5.rb,v 1.4 2001/07/13 15:38:27 knu Exp $
# $Id: md5.rb 12008 2007-03-06 10:12:12Z knu $

require 'digest/ext/md5/md5'

class MD5 < Digest::MD5
  class << self
    alias orig_new new
    def new(str = nil)
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
