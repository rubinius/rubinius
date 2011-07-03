class Hash
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Hash, :to_hash
  end

  alias_method :key, :index

  # Sets the default proc to be executed on each key lookup
  def default_proc=(proc)
    @default = Type.coerce_to(proc, Proc, :to_proc)
    @default_proc = true
  end

  alias_method :to_s, :inspect

  alias_method :each_pair, :each

  def merge!(other)
    Rubinius.check_frozen

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

end
