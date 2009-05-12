class Hash
  class << self
    def try_convert(x)
      return nil unless x.respond_to? :to_hash
      x.to_hash
    end
  end
end