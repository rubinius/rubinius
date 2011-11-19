require 'digest/ext/digest'

module Digest
  def self.const_missing(name)
    case name
    when :SHA256, :SHA384, :SHA512
      path = "digest/sha2"
    else
      lib = name.to_s.downcase
      path = "digest/#{lib}"
    end

    begin
      require path
    rescue LoadError => e
      raise LoadError, "library not found for class Digest::#{name} -- #{path}", caller(1)
    end
    unless Digest.const_defined?(name)
      raise NameError, "uninitialized constant Digest::#{name}", caller(1)
    end
    Digest.const_get(name)
  end

  class ::Digest::Class
    # creates a digest object and reads a given file, _name_.
    # 
    #  p Digest::SHA256.file("X11R6.8.2-src.tar.bz2").hexdigest
    #  # => "f02e3c85572dc9ad7cb77c2a638e3be24cc1b5bea9fdbb0b0299c9668475c534"
    def self.file(name)
      new.file(name)
    end
  end

  module Instance
    # updates the digest with the contents of a given file _name_ and
    # returns self.
    def file(name)
      File.open(name, "rb") {|f|
        buf = ""
        while f.read(16384, buf)
          update buf
        end
      }
      self
    end
  end
end

class Object
  def Digest(name)
    Digest.const_get(name)
  end

  private :Digest
end
