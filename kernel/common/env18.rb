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

    def set_encoding(value)
      value
    end

    private :set_encoding
  end
end
