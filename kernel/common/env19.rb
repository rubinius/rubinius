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

    def fetch(key, absent=undefined)
      if block_given? and !absent.equal?(undefined)
        warn "block supersedes default value argument"
      end

      if value = self[key]
        return value
      end

      if block_given?
        return yield(key)
      elsif absent.equal?(undefined)
        raise KeyError, "key not found"
      end

      return absent
    end

    def keep_if(&block)
      return to_enum(:keep_if) unless block_given?
      select!(&block)
      self
    end

    def select!
      return to_enum(:select!) unless block_given?
      reject! { |k, v| !yield(k, v) }
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
