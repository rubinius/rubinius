class Autoload
  attr_reader :name
  attr_reader :scope
  attr_reader :path

  def initialize(name, scope, path)
    @name = name
    @scope = scope
    @path = path
    Autoload.add(self)
  end

  def call
    Autoload.remove(path)
    require(path)
    scope.const_get(name)
  end

  def discard
    scope.__send__(:remove_const, name)
  end

  class << self
    def add(al)
      @autoloads ||= {}
      @autoloads[al.path] = al
    end

    def remove(path)
      @autoloads ||= {}
      return unless @autoloads.key?(path)
      al = @autoloads.delete(path)
      al.discard
    end
  end
end
