##
# Used to implement Module#autoload.

class Autoload
  attr_reader :name
  attr_reader :scope
  attr_reader :path
  attr_reader :original_path

  def initialize(name, scope, path)
    @name = name
    @scope = scope
    @original_path = path
    @path, = __split_path__(path)
    Autoload.add(self)
  end

  def call
    require(path)
    scope.const_get(name)
  end

  def discard
    scope.__send__(:remove_const, name)
  end

  class << self
    def autoloads
      @autoloads ||= {}
    end

    def add(al)
      autoloads[al.path] = al
    end

    def remove(path)
      al = autoloads.delete(path)
      al.discard if al
    end
  end
end
