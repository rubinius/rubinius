class Hash
  def self.[](*args)
    if args.size == 1
      obj = args.first
      if obj.kind_of? Hash
        return new.replace(obj)
      elsif obj.respond_to? :to_hash
        return new.replace(Type.coerce_to(obj, Hash, :to_hash))
      elsif obj.is_a?(Array) # See redmine # 1385
        h = {}
        args.first.each do |arr|
          next unless arr.respond_to? :to_ary
          arr = arr.to_ary
          next unless (1..2).include? arr.size
          h[arr.at(0)] = arr.at(1)
        end
        return h
      end
    end

    if args.size & 1 == 1
      raise ArgumentError, "Expected an even number, got #{args.length}"
    end

    hsh = new
    i = 0
    while i < args.size
      hsh[args[i]] = args[i+1]
      i += 2
    end
    hsh
  end

  def delete_if(&block)
    return to_enum :delete_if unless block_given?
    select(&block).each { |k, v| delete k }
    self
  end

  def each_key
    return to_enum :each_key unless block_given?
    i = to_iter
    while entry = i.next(entry)
      yield entry.key
    end
    self
  end

  def each
    return to_enum :each unless block_given?
    i = to_iter
    while entry = i.next(entry)
      yield [entry.key, entry.value]
    end
    self
  end

  def each_pair
    return to_enum :each_pair unless block_given?
    i = to_iter
    while entry = i.next(entry)
      yield entry.key, entry.value
    end
    self
  end

  def each_value
    return to_enum :each_value unless block_given?
    i = to_iter
    while entry = i.next(entry)
      yield entry.value
    end
    self
  end

  def reject(&block)
    return to_enum :reject unless block_given?
    hsh = dup
    hsh.reject! &block
    hsh
  end

  def reject!
    return to_enum :reject! unless block_given?
    rejected = select { |k, v| yield k, v }
    return if rejected.empty?

    rejected.each { |k, v| delete k }
    self
  end

  def select
    return to_enum :select unless block_given?
    selected = []
    i = to_iter
    while entry = i.next(entry)
      if yield(entry.key, entry.value)
        selected << [entry.key, entry.value]
      end
    end

    selected
  end
end
