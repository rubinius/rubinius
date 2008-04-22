class Autoload
  attr_reader :name
  attr_reader :scope
  attr_reader :path

  def initialize(name, scope, path)
    @name = name
    @scope = scope
    @path = path
  end

  def call
    scope.__send__(:remove_const, name)
    require(path)
    scope.const_get(name)
  end
end
