module OpenSSL
  class HMAC
    module Foreign
      set_ffi_lib "libcrypto"
      # HMAC(evp_md, key, key_len, data, data_len, md, buffer_length)
      attach_function "HMAC", :ossl_hmac,
            [:pointer, :string, :int, :string, :int, :string, :pointer], :pointer
    end

    def self.hexdigest(digest, key, data)
      unless digest.kind_of?(OpenSSL::Digest::Digest)
        msg = "wrong argument (#{digest.class})! " \
              "(Expected kind of OpenSSL::Digest::Digest)"
        raise TypeError, msg
      end
      key = StringValue(key)
      data = StringValue(data)

      md = digest.__send__(:message_digest_backend)
      buffer_size = MemoryPointer.new(:uint)
      buf = Foreign.ossl_hmac(md, key, key.size, data, data.size, nil, buffer_size)
      buf.read_string_as_hex(buffer_size.read_int)
    end
  end
end



