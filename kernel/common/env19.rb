module Rubinius
  class EnvironmentVariables
    def []=(key, value)
      key = StringValue(key)
      if value.nil?
        unsetenv(key)
      else
        if setenv(key, StringValue(value), 1) != 0
          Errno.handle("setenv")
        end
      end
      value
    end

    alias_method :store, :[]=

    alias_method :to_h, :to_hash

    def keep_if(&block)
      return to_enum(:keep_if) unless block_given?
      select!(&block)
      self
    end

    def select!(&block)
      return to_enum(:select!) unless block_given?
      reject! {|k, v| !block.call(k, v) }
    end

    def assoc(key)
      key = StringValue(key)
      value = self[key]
      value ? [key, value] : nil
    end

    def rassoc(value)
      value = StringValue(value)
      key = index(value)
      key ? [key, value] : nil
    end

    def set_encoding(value)
      return unless value.kind_of? String
      if Encoding.default_internal
        value.encode Encoding.default_internal, Encoding.find("locale")
      else
        value.force_encoding Encoding.find("locale")
      end
    end

    private :set_encoding
  end
end
