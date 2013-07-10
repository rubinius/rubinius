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
  attr_reader :constant
  attr_reader :thread

  def initialize(name, scope, path)
    @name = name
    @scope = scope
    @path = path
    @constant = undefined
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

    if !undefined.equal?(constant) && Thread.current == thread
      constant
    else
      worked = resolve

      if !honor_require or worked
        find_const under
      end
    end
  end

  def resolve
    Rubinius::CodeLoader.require @path
  end

  def find_const under
    current, constant = under, undefined

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
