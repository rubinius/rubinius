module Digest

 # Generates a hex-encoded version of a given _string_
  def Digest.hexencode(string)
    hex = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f']
    
    result = ' '*(string.length * 2)
    
    string.split('').to_a.each_with_index do |byte,i|
      byte = byte[0]
      result[i + i]     = hex[byte >> 4];
      result[i + i + 1] = hex[byte & 0x0f];
    end
    
    result
  end

  # This module provides instance methods for a digest implementation
  # object to calculate message digest values.
  class Instance

    # call-seq:
    #     digest_obj.update(string) -> digest_obj
    #     digest_obj << string -> digest_obj
    #
    # Updates the digest using a given _string_ and returns self.
    #
    # The update() method and the left-shift operator are overridden by
    # each implementation subclass. (One should be an alias for the
    # other)
    def update(string)
      raise RuntimeError, "#{self.class} does not implement update()"
    end
    alias :<< :update

 
    # call-seq:
    #     digest_obj.instance_eval { finish } -> digest_obj
    #
    # Finishes the digest and returns the resulting hash value.
    #
    # This method is overridden by each implementation subclass and often
    # made private, because some of those subclasses may leave internal
    # data uninitialized.  Do not call this method from outside.  Use
    # #digest!() instead, which ensures that internal data be reset for
    # security reasons.
    def finish(string)
      raise RuntimeError, "#{self.class} does not implement finish()"
    end

    #
    # call-seq:
    #     digest_obj.reset -> digest_obj
    #
    # Resets the digest to the initial state and returns self.
    #
    # This method is overridden by each implementation subclass.
    def reset(string)
      raise RuntimeError, "#{self.class} does not implement reset()"
    end

    # call-seq:
    #     digest_obj.new -> another_digest_obj
    #
    # Returns a new, initialized copy of the digest object.  Equivalent
    # to digest_obj.clone().reset().
    def new
      self.clone.reset
    end

    # call-seq:
    #     digest_obj.digest -> string
    #     digest_obj.digest(string) -> string
    #
    # If none is given, returns the resulting hash value of the digest,
    # keeping the digest's state.
    #
    # If a _string_ is given, returns the hash value for the given
    # _string_, resetting the digest to the initial state before and
    # after the process.
    def digest(string=nil)
      if (string) 
        reset
        update(string)
        value = finish
        reset
      else
        clone = self.clone
        value = clone.finish
        clone.reset
      end
      
      value
    end

    # call-seq:
    #     digest_obj.digest! -> string
    #
    # Returns the resulting hash value and resets the digest to the
    # initial state.
    def digest!
      value = finish
      reset
      value
    end

    # call-seq:
    #     digest_obj.hexdigest -> string
    #     digest_obj.hexdigest(string) -> string
    #
    # If none is given, returns the resulting hash value of the digest in
    # a hex-encoded form, keeping the digest's state.
    #
    # If a _string_ is given, returns the hash value for the given
    # _string_ in a hex-encoded form, resetting the digest to the initial
    # state before and after the process.
    def hexdigest(data=nil)
      Digest.hexencode(digest(data))
    end

    # call-seq:
    #     digest_obj.hexdigest! -> string
    #
    # Returns the resulting hash value and resets the digest to the
    # initial state.
    def hexdigest!
      result = hexdigest()
      reset()
      result
    end

    # call-seq:
    #     digest_obj.to_s -> string
    #
    # Returns digest_obj.hexdigest().
    def to_s
      hexdigest
    end

    # call-seq:
    #     digest_obj.inspect -> string
    #
    # Creates a printable version of the digest object.
    def inspect
      "#<#{self.class}: #{self.hexdigest}>"
    end

    # call-seq:
    #     digest_obj == another_digest_obj -> boolean
    #     digest_obj == string -> boolean
    #
    # If a string is given, checks whether it is equal to the hex-encoded
    # digest value of the digest object.  If another digest instance is
    # given, checks whether they have the same digest value.  Otherwise
    # returns false.
    def ==(other)
      return hexdigest == other.hexdigest if other.is_a? Digest::Instance
      to_s == other.to_str
    end
    
    # call-seq:
    #     digest_obj.digest_length -> integer
    #
    # Returns the length of the hash value of the digest.
    #
    # This method should be overridden by each implementation subclass.
    # If not, digest_obj.digest().length() is returned.
    def digest_length
      digest.length
    end

    # call-seq:
    #     digest_obj.length -> integer
    #     digest_obj.size -> integer
    #
    # Returns digest_obj.digest_length().
    alias :length :digest_length
    alias :size   :digest_length
    
    # call-seq:
    #     digest_obj.block_length -> integer
    #
    # Returns the block length of the digest.
    #
    # This method is overridden by each implementation subclass.
    def block_length
      raise RuntimeError, "#{self.class} does not implement block_length()"
    end

  end

  class Class
    # call-seq:
    #     Digest::Class.digest(string, #parameters) -> hash_string
    #
    # Returns the hash value of a given _string_.  This is equivalent to
    # Digest::Class.new(#parameters).digest(string), where extra
    # _parameters_, if any, are passed through to the constructor and the
    # _string_ is passed to #digest().
    def digest(data)
      raise ArgumentError, "no data given" unless data
      self.new.digest(data)
    end
    
    # call-seq:
    #     Digest::Class.hexdigest(string[, ...]) -> hash_string
    #
    # Returns the hex-encoded hash value of a given _string_.  This is
    # almost equivalent to
    # Digest.hexencode(Digest::Class.new(*parameters).digest(string)).
    def hexdigest(string = nil)
      data = nil

      if string then
        reset
        update string
        data = finish
        reset
      else
        new = clone
        data = new.send :finish
        new.reset
      end

      Digest.hexencode data
    end
   
  end
  
end
