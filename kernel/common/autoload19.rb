# -*- encoding: us-ascii -*-

class Autoload
  ##
  # When any code that finds a constant sees an instance of Autoload as its match,
  # it calls this method on us
  def call(load_scope, honor_require)
    # We leave the Autoload object in the constant table so that if another
    # thread hits this while we're mid require they'll be come in here and
    # be held by require until @path is available, at which time they'll
    # attempt the lookup again.
    #
    worked = Rubinius::CodeLoader.require @path

    if !honor_require or worked
      find_const load_scope
    end
  end

  def find_const load_scope
    current, constant = load_scope, undefined

    while current
      constant = current.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        if constant.equal? self
          constant = Object.constant_table.fetch name, undefined
          if constant.equal?(undefined)
            return load_scope.const_missing(name)
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
          return load_scope.const_missing(name)
        end
        return constant
      end
    end

    @scope.const_missing(name)
  end
end
