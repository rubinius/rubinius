class Dir
  def self.allocate
    Rubinius.primitive :dir_allocate
    raise PrimitiveFailure, "Dir.allocate primitive failed"
  end

  def initialize(path, options=undefined)
    path = Rubinius::Type.coerce_to_path path

    if options.equal? undefined
      enc = nil
    else
      options = Rubinius::Type.coerce_to options, Hash, :to_hash
      enc = options[:encoding]
      enc = Rubinius::Type.coerce_to_encoding enc if enc
    end

    Rubinius.invoke_primitive :dir_open, self, path, enc
  end

  private :initialize

  def close
    Rubinius.primitive :dir_close
    raise PrimitiveFailure, "Dir#close primitive failed"
  end

  def closed?
    Rubinius.primitive :dir_closed_p
    raise PrimitiveFailure, "Dir#closed? primitive failed"
  end

  def read
    entry = Rubinius.invoke_primitive :dir_read, self
    return unless entry

    enc = Encoding.default_internal
    enc ? entry.encode(enc) : entry
  end

  def control(kind, pos)
    Rubinius.primitive :dir_control
    raise PrimitiveFailure, "Dir#__control__ primitive failed"
  end

  private :control
end
