##
# Used to implement Module#autoload.

class Autoload
  attr_reader :name
  attr_reader :scope
  attr_reader :path
  attr_reader :constant

  attr_accessor :loaded
  attr_accessor :loading

  def self.allocate
    Rubinius.primitive :autoload_allocate
    raise PrimtiveFailure, "Autoload.allocate primitive failed"
  end

  def initialize(name, scope, path)
    @name = name
    @scope = scope
    @path = path
    @constant = undefined
    @loading = false
    @loaded = false
  end

  private :initialize

  def inspect
    "#<#{self.class.name}:0x#{object_id.to_s(16)} name=#{@name} scope=#{@scope}" \
      " path=#{@path} constant=#{@constant} loading=#{@loading} loaded=#{@loaded}>"
  end

  alias_method :to_s, :inspect

  def loading?
    Rubinius.synchronize(self) { @loading }
  end

  ##
  #
  # Change the file to autoload. Used by Module#autoload
  def set_path(path)
    @path = path
    @loading = false
  end

  ##
  # When any code that finds a constant sees an instance of Autoload as its match,
  # it calls this method on us
  def call(under, honor_require=false)
    return constant unless undefined.equal? constant

    if resolve or not honor_require
      find_const under
    end
  end

  def resolve
    # The protocol that MRI defines for resolving an Autoload instance
    # requires that the constant table entry be removed _during_ the load so
    # that Module#const_defined? returns false and defined?() returns nil for
    # the constant that triggered the load.

    Rubinius.synchronize(self) do
      unless @loaded or @loading
        @loading = true

        result = Rubinius::CodeLoader.new(@path).require

        @loaded = true if result
        @loading = false

        result
      end
    end
  end

  def find_const(under)
    current = under
    constant = undefined

    while current
      if entry = current.constant_table.lookup(name)
        constant = entry.constant
        if constant.equal? self
          if undefined.equal?(constant.constant)
            unless Object.constant_table.lookup(name)
              return under.const_missing(name)
            end
          else
            entry.constant = constant.constant
            return constant.constant
          end
        end
        return constant
      end

      current = current.direct_superclass
    end

    if instance_of?(Module)
      if entry = Object.constant_table.lookup(name)
        constant = entry.constant
        if constant.equal? self
          if undefined.equal?(constant.constant)
            return under.const_missing(name)
          else
            entry.constant = constant.constant
            return constant.constant
          end
        end
        return constant
      end
    end

    under.const_missing(name)
  end
end
