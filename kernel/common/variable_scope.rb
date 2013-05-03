# -*- encoding: us-ascii -*-

module Rubinius
  class VariableScope
    attr_reader :parent
    attr_reader :block
    attr_reader :module
    attr_reader :self

    # CompiledCode this scope is for.
    #
    attr_reader :method

    def self=(obj)
      @self = obj
    end

    def has_dynamic_locals?
      @dynamic_locals
    end

    def dynamic_locals
      @dynamic_locals ||= Rubinius::LookupTable.new
    end

    def set_eval_local(name, val)
      scope = @parent
      while scope
        if scope.dynamic_locals.key? name
          scope.dynamic_locals[name] = val
          return val
        end

        scope = scope.parent
      end

      # Otherwise, put it in the highest non-eval scope
      if @parent
        scope = @parent
        while scope.parent and scope.method.for_eval?
          scope = scope.parent
        end

        scope.dynamic_locals[name] = val
      else
        # This happens when we run some confused compiled code.
        # Typically when we use eval to create the code and then
        # reuse it in a non-eval context.
        dynamic_locals[name] = val
      end
    end

    def get_eval_local(name)
      scope = self
      while scope
        if scope.has_dynamic_locals? && scope.dynamic_locals.key?(name)
          return scope.dynamic_locals[name]
        end

        scope = scope.parent
      end

      nil
    end

    def eval_local_defined?(name, search=true)
      if search
        scope = self
        while scope
          return true if scope.dynamic_locals.key? name

          scope = scope.parent
        end
      else
        return true if @dynamic_locals and @dynamic_locals.key? name
      end

      return false
    end

    def local_defined?(name)
      vars = self
      while vars
        return true if vars.method.local_names.include? name
        return true if vars.eval_local_defined?(name, false)
        vars = vars.parent
      end
    end

    def local_layout
      vars = self
      out = []

      while vars
        meth = vars.method.local_names.to_a
        eloc = vars.dynamic_locals.keys.map { |x| x.to_s }.sort

        out << [meth, eloc]

        vars = vars.parent
      end

      out
    end

    def exitted?
      @exitted
    end

    def super_method_defined?
      # Minor HACK __block__ as the name is ambigious. Need a better
      # way to detect that a VS is for a define_method scope.
      #
      # This code is so that if this VS is for a block, we walk up
      # to the VS for the actual method. It's complicated by the fact
      # that there may be a VS for a define_method in there, so we need
      # to stop looking if we hit one.
      if @parent and @method.name == :__block__
        vars = self

        while vars.method.name == :__block__ and nxt = vars.parent
          vars = nxt
        end

        return vars.super_method_defined?
      end

      if sup = @module.direct_superclass
        # This will probably break for define_method
        return sup.lookup_method(@method.name) != nil
      end

      return false
    end

    def method_visibility
      return @method_visibility if @method_visibility

      # if this scope is for a script, and there is no method_visibility
      # already set, then we default to private.
      #
      # This is so that a script body has it's visibility default to private.

      return :private if script?
      return nil if top_level_visibility?
      return @parent.method_visibility if @parent

      # The default, let the caller sort it out.
      return nil
    end

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} module=#{@module} method=#{@method.inspect}>"
    end
  end
end
