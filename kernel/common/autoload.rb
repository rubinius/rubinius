# -*- encoding: us-ascii -*-

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

  ##
  # When any code that finds a constant sees an instance of Autoload as its match,
  # it calls this method on us
  def call(under, honor_require=false)
    # We leave the Autoload object in the constant table so that if another
    # thread hits this while we're mid require they'll be come in here and
    # be held by require until @path is available, at which time they'll
    # attempt the lookup again.
    #
    worked = resolve

    if !honor_require or worked
      find_const under
    end
  end

  def resolve
    Rubinius::CodeLoader.require @path
  end

  def find_const under
    current, constant = under, undefined

    while current
      constant = current.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        if constant.equal? self
          constant = Object.constant_table.fetch name, undefined
          if constant.equal?(undefined)
            return under.const_missing(name)
          end
        end
        return constant
      end

      current = current.direct_superclass
    end

    if instance_of?(Module)
      constant = Object.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        if constant.equal? self
          return under.const_missing(name)
        end
        return constant
      end
    end

    under.const_missing(name)
  end

end
