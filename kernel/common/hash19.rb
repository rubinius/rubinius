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
end
