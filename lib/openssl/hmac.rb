module OpenSSL
  class HMAC
    module Foreign
      set_ffi_lib "libcrypto"
      # HMAC(evp_md, key, key_len, data, data_len, md, buffer_length)
      attach_function "HMAC", :ossl_hmac,
            [:pointer, :string, :int, :string, :int, :string, :pointer], :pointer
    end

    def self.digest(md, key, data)
      unless md.kind_of?(OpenSSL::Digest::Digest)
        msg = "wrong argument (#{md.class})! " \
              "(Expected kind of OpenSSL::Digest::Digest)"
        raise TypeError, msg
      end
      key = StringValue(key)
      data = StringValue(data)

      evp_md = md.__send__(:message_digest_backend)
      buffer_size = MemoryPointer.new(:uint)
      buf = Foreign.ossl_hmac(evp_md, key, key.size, data, data.size, nil, buffer_size)
      buf.read_string(buffer_size.read_int)
    end

    def self.hexdigest(md, key, data)
      OpenSSL.digest_to_hex digest(md, key, data)
    end
  end
end



