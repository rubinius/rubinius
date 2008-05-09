module OpenSSL
  module Digest
    # Native OpenSSL functions and structs
    module Foreign
      set_ffi_lib "libssl"
      # const EVP_MD *EVP_get_digestbyname(const char *name);
      attach_function "EVP_get_digestbyname", "ossl_digest_getbyname", [:string], :pointer

      #void OpenSSL_add_all_digests(void);
      attach_function "OpenSSL_add_all_digests", "ossl_add_digests", [], :void

      # int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
      attach_function "EVP_DigestInit_ex", "ossl_digest_init_ex", [:pointer, :pointer, :pointer], :int

      #int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *d, size_t cnt);
      attach_function "EVP_DigestUpdate", "ossl_digest_update", [:pointer, :string, :size_t], :int

      #EVP_MD_CTX *EVP_MD_CTX_create(void);
      attach_function "EVP_MD_CTX_create", "ossl_digest_ctx_create", [], :pointer

      #int EVP_MD_CTX_copy_ex(EVP_MD_CTX *out,const EVP_MD_CTX *in);
      attach_function "EVP_MD_CTX_copy_ex", "ossl_digest_ctx_copy", [:pointer, :pointer], :int

      #int EVP_MD_CTX_cleanup(EVP_MD_CTX *ctx);
      attach_function "EVP_MD_CTX_cleanup", "ossl_digest_ctx_cleanup", [:pointer], :int

      #int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *s);
      attach_function "EVP_DigestFinal_ex", "ossl_digest_final_ex", [:pointer, :pointer, :pointer], :int

      class EVP_MD < FFI::Struct
        config("rbx.platform.evp_md", :md_size)
      end

      class EVP_MD_CTX < FFI::Struct
        config("rbx.platform.evp_md_ctx", :digest)
        def digest_size
          evp_md = EVP_MD.new self[:digest]
          evp_md[:md_size]
        end
      end
    end # Foreign

    class Digest
      # Populate OpenSSL's digest type table
      Foreign.ossl_add_digests

      class << self
        # We need to bypass this version of 'new' in subclasses
        alias_method :original_new, :new

        def new(digest_type)
          digest_type = digest_type.to_s
          begin
            digest_class = OpenSSL::Digest::const_get(digest_type)
          rescue NameError
            raise RuntimeError, "Unsupported digest algorithm (#{digest_type})."
          end
          return digest_class.new
        end
      end # metaclass

      def initialize(data = nil)
        @context, @digest = nil, nil

        @context = Foreign.ossl_digest_ctx_create

        @digest = Foreign.ossl_digest_getbyname self.class.const_get(:DigestName)
        Errno.handle if @digest.nil?
        Foreign.ossl_digest_init_ex(@context, @digest, nil)
        update(data) if data
      end

      def update(data)
        data = StringValue(data)
        err = Foreign.ossl_digest_update @context, data, data.size
        Errno.handle if err.zero?
        return self
      end

      def hexdigest
        buffer = finalize

        out = ""
        # Convert the binary string into lowercase hex
        buffer.read_string(buffer.total).each_byte do |byte|
          # TODO - Implement a lightweight helper for hex values
          # with leading zeroes. sprintf is slow.
          out << sprintf("%02x", byte)
        end
        out
      end
      alias_method :to_s, :hexdigest
      alias_method :inspect, :hexdigest

      # Copy the digest context and then finalize it
      def finalize
        # Create a placeholder to copy into
        # TODO - ctx_create may be overkill here. Just use ctx_init?
        final = Foreign.ossl_digest_ctx_create

        err = Foreign.ossl_digest_ctx_copy(final, @context)
        Errno.handle if err.zero?

        final_ctx = Foreign::EVP_MD_CTX.new(final)
        # Create a buffer to hold the finalized output
        buffer = MemoryPointer.new(final_ctx.digest_size)
        buffer_size = MemoryPointer.new(:uint)
        buffer_size.write_int(buffer.total)

        err = Foreign.ossl_digest_final_ex(final, buffer, buffer_size)
        Foreign.ossl_digest_ctx_cleanup(final)
        if err.zero? then
          Errno.handle
        else
          buffer
        end
      end
      private :finalize
    end # Digest

    class SHA1 < Digest
      DigestName = "SHA1"
      def self.new(*args)
        original_new(*args)
      end
    end # SHA1
  end # OpenSSL::Digest
end # OpenSSL
