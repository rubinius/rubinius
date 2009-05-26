# These methods are overriden by lib/1.8.7/... or lib/1.9/...
module Rubinius
  class EnvironmentVariables
    def delete_if(&block)
      reject!(&block)
      self
    end

    def each_key
      each { |k, v| yield k }
    end

    def each_value
      each { |k, v| yield v }
    end

    def reject!
      rejected = false
      each do |k, v|
        if yield k, v
          delete k
          rejected = true
        end
      end
      rejected ? self : nil
    end

    def each
      to_hash.each { |k, v| yield k, v }
      self
    end
    alias_method :each_pair, :each

  end
end