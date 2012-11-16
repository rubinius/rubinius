# -*- encoding: us-ascii -*-

##
# A linked list that details the static, lexical scope the method was created
# in.
#
# TODO: document

module Rubinius
  class ConstantScope

    #
    # @todo  Verify the recursion here does not cause problems. --rue
    #
    def initialize(mod, par = nil)
      @module = mod
      @parent = par
    end

    # Source code of this scope.
    attr_accessor :script

    # Module or class this lexical scope enclosed into.
    attr_reader   :module

    # Constant scope object this scope enclosed into.
    attr_reader   :parent

    # Module or class representing the 'current class'. MRI manipulates
    # this outside of the lexical scope and uses it for undef and method
    # definition.
    attr_accessor :current_module

    # Set to indicate that no methods may be add to this scope
    attr_accessor :disabled_for_methods

    # Lazy initialized hash map used for flip-flops
    attr_accessor :flip_flops

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} parent=#{@parent.inspect} module=#{@module}>"
    end

    def to_s
      inspect
    end

    # Indicates if this is a toplevel/default scope
    def top_level?
      !@parent
    end

    # Use the same info as the current ConstantScope, but set current_module to
    # +mod+. Chains off the current ConstantScope.
    def using_current_as(mod)
      if top_level?
        # Don't chain up if this is a toplevel, create a new one.
        cs = dup
      else
        cs = ConstantScope.new @module, self
      end

      cs.current_module = mod
      return cs
    end

    def using_disabled_scope
      cs = using_current_as(@module)
      cs.disabled_for_methods = true
      return cs
    end

    def for_method_definition
      if @disabled_for_methods
        raise TypeError, "unable to create methods in this scope"
      end

      return @current_module if @current_module
      return @module
    end

    def alias_method(name, original)
      Rubinius.privately do
        for_method_definition.alias_method name, original
      end
    end

    def __undef_method__(name)
      Rubinius.privately do
        for_method_definition.undef_method name
      end
    end

    def active_path
      script = current_script
      if script
        if path = script.file_path
          return path.dup
        end
      end

      return "__unknown__.rb"
    end

    def absolute_active_path
      script = current_script
      if script
        if path = script.data_path
          return path.dup
        end
      end

      return nil
    end

    def data_path
      script = current_script
      if script and script.main?
        if path = script.data_path
          return path.dup
        end
      end

      return nil
    end

    def current_script
      scope = self
      while scope and !scope.script
        scope = scope.parent
      end
      return scope && scope.script
    end

    def const_set(name, value)
      @module.const_set name, value
    end
  end
end

