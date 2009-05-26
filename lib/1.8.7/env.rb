module Rubinius
  class EnvironmentVariables
    def delete_if(&block)
      return to_enum :delete_it unless block_given?
      reject!(&block)
      self
    end

    def each_key
      return to_enum :each_key unless block_given?
      each { |k, v| yield k }
    end

    def each_value
      return to_enum :each_value unless block_given?
      each { |k, v| yield v }
    end

    def reject!
      return to_enum :reject! unless block_given?
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
      return to_enum :each unless block_given?
      to_hash.each { |k, v| yield k, v }
      self
    end
    alias_method :each_pair, :each

  end
end