module OpenSSL
  class Random

    module Foreign
      extend FFI::Library

      ffi_lib "libcrypto"

      # RAND_bytes(unsigned char* buf, int num)
      attach_function :ossl_bytes, "RAND_bytes", [:string, :int], :int

      # RAND_pseudo_bytes(unsigned char* buf, int num)
      attach_function :ossl_pseudo_bytes, "RAND_pseudo_bytes", [:string, :int], :int
    end

    def self.random_bytes(n)
      if n < 0
        raise ArgumentError.new('negative string size')
      end

      random_string = ' ' * n
      ret = Foreign.ossl_bytes(random_string, n)

      raise OpenSSLError.new('random_bytes unsupported by current RAND method') if ret == -1
      raise OpenSSLError.new('call to RAND_bytes(3) failed') if ret == 0

      random_string
    end

    def self.pseudo_bytes(n)
      if n < 0
        raise ArgumentError.new('negative string size')
      end

      random_string = ' ' * n
      ret = Foreign.ossl_pseudo_bytes(random_string, n)

      raise OpenSSLError.new('random_bytes unsupported by current RAND method') if ret == -1
      # per man RAND_bytes(3), RAND_pseudo_bytes doesn't appear to
      # signal errors -- the return value signifies if the bytes
      # returned are cryptographically secure, NOT if the function
      # succeeded or not ...

      random_string
    end
  end
end
