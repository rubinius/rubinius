class Autoload
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
  rescue NameError, LoadError => e
    scope.constant_table[@name] = self
    Rubinius.inc_global_serial
    
    raise e
  end
end
