module Rubinius
  class Mirror
    module Process
      def self.set_status_global(status)
        ::Thread.current[:$?] = status
      end

      def self.fork
        Rubinius.primitive :vm_fork
        raise PrimitiveFailure, "Rubinius::Mirror::Process.fork primitive failed"
      end

      def self.exec(command, args)
        Rubinius.primitive :vm_exec
        raise PrimitiveFailure, "Rubinius::Mirror::Process.exec primitive failed"
      end

      def self.spawn(command, args)
        begin
          args.unshift command unless args.empty?
          pid = Rubinius.invoke_primitive :vm_spawn, command, args
        rescue SystemCallError => error
          set_status_global ::Process::Status.new(pid, 127)
          raise error
        end

        pid
      end

      def self.backtick(str)
        Rubinius.primitive :vm_backtick
        raise PrimitiveFailure, "Rubinius::Mirror::Process.backtick primitive failed"
      end
    end
  end
end
