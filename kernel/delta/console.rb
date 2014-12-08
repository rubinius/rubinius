module Rubinius
  module Console
    class Server
      # The following constants are set internally:
      # RequestPath - the path to the console request file for this process
      # ResponsePath - the path to the console response file for this process

      def evaluate(source)
        begin
          eval(source, TOPLEVEL_BINDING, "(console)", 1).inspect
        rescue Object => e
          e.message + e.backtrace.join("\n")
        end
      end
    end
  end
end
