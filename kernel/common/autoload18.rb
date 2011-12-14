class Autoload
  ##
  # When any code that finds a constant sees an instance of Autoload as its match,
  # it calls this method on us
  def call(honor_require=false)
    # We leave the Autoload object in the constant table so that if another
    # thread hits this while we're mid require they'll be come in here and
    # be held by require until @path is available, at which time they'll
    # attempt the lookup again.
    #
    worked = false
    attempted = false

    begin
      worked = Rubinius::CodeLoader.require @path
    else
      attempted = true
    ensure
      unless attempted
        # Because it didn't work out, we remove it now
        # rather than before.
        scope.constant_table.delete @name
        Rubinius.inc_global_serial
      end
    end

    if !honor_require or worked
      find_const
    end
  end

  def find_const
    current, constant = @scope, undefined

    while current
      constant = current.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        if constant.equal? self
          @scope.constant_table.delete @name
          Rubinius.inc_global_serial
          return scope.const_missing(name)
        end
        return constant
      end

      current = current.direct_superclass
    end

    if instance_of?(Module)
      constant = Object.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        if constant.equal? self
          @scope.constant_table.delete @name
          Rubinius.inc_global_serial
          return scope.const_missing(name)
        end
        return constant
      end
    end

    @scope.constant_table.delete @name
    Rubinius.inc_global_serial

    scope.const_missing(name)
  end
end
