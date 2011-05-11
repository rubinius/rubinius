##
# Used to implement Module#autoload.

class Autoload
  def self.allocate
    Ruby.primitive :autoload_allocate
    raise PrimtiveFailure, "Autoload.allocate primitive failed"
  end

  attr_reader :name
  attr_reader :scope
  attr_reader :path

  def initialize(name, scope, path)
    @name = name
    @scope = scope
    @path = path
  end

  ##
  # When any code that finds a constant sees an instance of Autoload as its match,
  # it calls this method on us
  def call(honor_require=false)
    # Remove the autoload object from the constant table it was in, so
    # we don't recurse back into ourself.
    scope.constant_table.delete @name
    Rubinius.inc_global_serial

    worked = Rubinius::CodeLoader.require @path
    if !honor_require or worked
      scope.const_get @name
    end
  end

  ##
  #
  # Change the file to autoload. Used by Module#autoload
  def set_path(path)
    @path = path
  end
end
