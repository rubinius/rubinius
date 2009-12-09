module OpenSSL
  autoload :HMAC, "openssl/hmac"
  autoload :Digest, "openssl/digest"
  autoload :Random, "openssl/random"

  class OpenSSLError < StandardError ; end

  # Used by the various hexdigest implementations
  def self.digest_to_hex(digest)
    out = ""
    digest.each_byte do |byte|
      hex = byte.to_s(16)
      out << "0" if byte < 0x10 # add leading zero if necessary
      out << hex
    end
    out
  end
end
