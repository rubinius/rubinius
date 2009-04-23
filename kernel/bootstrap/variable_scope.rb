module Rubinius
  class VariableScope
    def self.of_sender
      Ruby.primitive :variable_scope_of_sender
      raise PrimitiveFailure, "Unable to get VariableScope of sender"
    end

    def self.current
      Ruby.primitive :variable_scope_current
      raise PrimitiveFailure, "Unable to get current VariableScope"
    end

    def locals
      Ruby.primitive :variable_scope_locals
      raise PrimitiveFailure, "Unable to get VariableScope locals"
    end

    # To handle Module#private, protected
    attr_accessor :method_visibility

    # To handle $~, $1, etc

    def last_match=(match)
      if parent
        scope = self
        while scope.parent
          scope = scope.parent
        end
        scope.last_match = match
      else
        @last_match = match
      end

      match
    end

    def last_match
      if parent
        scope = self
        while scope.parent
          scope = scope.parent
        end

        scope.last_match
      else
        @last_match
      end
    end
    ##
    # The Nth group of the last regexp match.

    def nth_ref(n)
      if lm = last_match()
        return lm[n]
      end

      return nil
    end

    ##
    # One of the special globals $&, $`, $' or $+.

    def back_ref(kind)
      if lm = last_match()
        res = case kind
        when :&
          lm[0]
        when :"`"
          lm.pre_match
        when :"'"
          lm.post_match
        when :+
          lm.captures.last
        end

        return res
      end

      return nil
    end
  end
end
