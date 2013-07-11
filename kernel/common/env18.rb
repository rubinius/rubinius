module Rubinius
  class EnvironmentVariables
    def []=(key, value)
      key = StringValue(key)
      if value.nil?
        unsetenv(key)
      else
        setenv key, StringValue(value), 1
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
        raise IndexError, "key not found"
      end

      return absent
    end

    def set_encoding(value)
      value
    end

    private :set_encoding
  end
end
