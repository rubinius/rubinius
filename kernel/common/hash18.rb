class Hash
  def each_pair
    return to_enum(:each_pair) unless block_given?

    each_entry do |entry|
      yield entry.key, entry.value
    end
    self
  end

  def merge!(other)
    other = Rubinius::Type.coerce_to other, Hash, :to_hash

    if block_given?
      other.each_entry do |entry|
        key = entry.key
        if key? key
          __store__ key, yield(key, self[key], entry.value)
        else
          __store__ key, entry.value
        end
      end
    else
      other.each_entry do |entry|
        key = entry.key
        __store__ key, entry.value
      end
    end
    self
  end
  alias_method :update, :merge!

  def to_s
    to_a.join
  end
end
