module Rubinius
  class EnvironmentVariables
    def set_encoding(value)
      return unless value.kind_of? String
      value.force_encoding Encoding.find("locale")
    end

    private :set_encoding
  end
end
