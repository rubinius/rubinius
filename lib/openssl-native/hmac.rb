module OpenSSL
  class HMAC
    module Foreign
      extend FFI::Library

      ffi_lib "libcrypto"

      # HMAC(evp_md, key, key_len, data, data_len, md, buffer_length)
      attach_function :ossl_hmac, "HMAC",
            [:pointer, :string, :int, :string, :int, :string, :pointer], :pointer
    end

    # Creates a new HMAC object and returns the binary digest
    def self.digest(md, key, data)
      unless md.kind_of?(OpenSSL::Digest::Digest)
        msg = "wrong argument (#{md.class})! " \
              "(Expected kind of OpenSSL::Digest::Digest)"
        raise TypeError, msg
      end
      key = StringValue(key)
      data = StringValue(data)

      # Fetch the message digest structure from the Digest argument
      evp_md = md.__send__(:message_digest_backend)
      buffer_size = FFI::MemoryPointer.new(:uint)

      buf = Foreign.ossl_hmac(evp_md, key, key.size, data, data.size, nil, buffer_size)
      buf.read_string(buffer_size.read_int)
    end

    # Creates a new HMAC object and returns the digest as lowercase hex
    def self.hexdigest(md, key, data)
      OpenSSL.digest_to_hex digest(md, key, data)
    end
  end
end



