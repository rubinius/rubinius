module Rubinius
  class Console
    def evaluate(source)
      eval(source, TOPLEVEL_BINDING, "(console)", 1).inspect
    end
  end
end
