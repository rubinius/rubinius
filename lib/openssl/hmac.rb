module OpenSSL
  class HMAC
    set_ffi_lib "libssl"
    #attach_function "HMAC", :ossl_hmac, [:pointer, :pointer, :int, :string, :int, :string, :uint], :string

    def self.hexdigest(digest, key, data)
      warn "HMAC.hexdigest not implemented. Using placeholder."
      return data
    end
  end
end

__END__

static VALUE
ossl_hmac_s_hexdigest(VALUE klass, VALUE digest, VALUE key, VALUE data)
{
    char *buf, *hexbuf;
    int buf_len;
    VALUE hexdigest;

    StringValue(key);
    StringValue(data);
	
    buf = HMAC(GetDigestPtr(digest), RSTRING(key)->ptr, RSTRING(key)->len, RSTRING(data)->ptr, RSTRING(data)->len, NULL, &buf_len);
    if (string2hex(buf, buf_len, &hexbuf, NULL) != 2 * buf_len) {
      ossl_raise(eHMACError, "Cannot convert buf to hexbuf");
    }
    hexdigest = ossl_buf2str(hexbuf, 2 * buf_len);

    return hexdigest;
}


unsigned char *HMAC(const EVP_MD *evp_md, const void *key,
                      int key_len, const unsigned char *d, int n,
                      unsigned char *md, unsigned int *md_len);

