module Rubinius
  class EnvironmentVariables
    def keep_if(&block)
      return to_enum(:keep_if) unless block_given?
      reject! {|k, v| !block.call(k, v) }
      self
    end

    def set_encoding(value)
      return unless value.kind_of? String
      value.force_encoding Encoding.find("locale")
    end

    private :set_encoding
  end
end
