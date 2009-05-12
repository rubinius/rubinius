# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Hash
  def self.[](*args)
    if args.size == 1
      obj = args.first
      if obj.kind_of? Hash
        return new.replace(obj)
      elsif obj.respond_to? :to_hash
        return new.replace(Type.coerce_to(obj, Hash, :to_hash))
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
    select(&block).each { |k, v| delete k }
    self
  end

  def each_key
    each_item { |k, v| yield k }
    self
  end

  def each
    each_item { |k, v| yield [k, v] }
    self
  end

  # Yields key, value for each item in the Hash. This method
  # is necessary to protect the essential iterator from subclasses
  # (e.g. REXML::Attribute) that replace #each with a version
  # that is incompatible with the dependencies here (e.g. defining
  # #each -> #each_attribute -> #each_value, where we had been
  # defining #each_value in terms of #each).
  def each_item
    i = to_iter
    while entry = i.next
      begin
        yield entry.key, entry.value
      end while entry = entry.next
    end
    self
  end

  alias_method :each_pair, :each_item

  def each_value
    each_item { |k, v| yield v }
    self
  end

  def reject(&block)
    hsh = dup
    hsh.reject! &block
    hsh
  end

  def reject!
    rejected = select { |k, v| yield k, v }
    return if rejected.empty?

    rejected.each { |k, v| delete k }
    self
  end

  def select
    selected = []
    i = to_iter
    while e = i.next
      begin
        selected << [e.key, e.value] if yield(e.key, e.value)
      end while e = e.next
    end

    selected
  end
end