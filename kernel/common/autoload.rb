##
# Used to implement Module#autoload.

class Autoload
  def self.allocate
    Rubinius.primitive :autoload_allocate
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
  #
  # Change the file to autoload. Used by Module#autoload
  def set_path(path)
    @path = path
  end
end
