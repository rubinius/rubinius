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

      def self.exec(*args)
        exe = Execute.new(*args)
        exe.setup_process
        exe.exec exe.command, exe.argv
      end

      def self.spawn(*args)
        exe = Execute.new(*args)

        begin
          pid = exe.spawn exe, exe.command, exe.argv
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

      class Execute
        attr_reader :command
        attr_reader :argv

        # Turns the various varargs incantations supported by Process.spawn into a
        # [env, prog, argv, redirects, options] tuple.
        #
        # The following method signature is supported:
        #   Process.spawn([env], command, ..., [options])
        #
        # The env and options hashes are optional. The command may be a variable
        # number of strings or an Array full of strings that make up the new process's
        # argv.
        #
        # Assigns @environment, @command, @argv, @redirects, @options. All
        # elements are guaranteed to be non-nil. When no env or options are
        # given, empty hashes are returned.
        def initialize(env_or_cmd, *args)
          @arguments = args

          if @options = Rubinius::Type.try_convert(args.last, ::Hash, :to_hash)
            args.pop
          else
            @options = {}
          end

          if @environment = Rubinius::Type.try_convert(env_or_cmd, ::Hash, :to_hash)
            unless @command = args.shift
              raise ArgumentError, "command argument expected"
            end
          else
            @command = env_or_cmd
            @environment = {}
          end

          process_environment
          process_command
          process_options
        end

        def process_environment
          adjusted = {}

          @environment.each do |key, value|
            key   = Rubinius::Type.check_null_safe(StringValue(key))
            value = Rubinius::Type.check_null_safe(StringValue(value)) unless value.nil?

            if key.include?("=")
              raise ArgumentError, "environment name contains a equal : #{key}"
            end

            adjusted[key] = value
          end

          @environment = adjusted
        end

        # Mapping of string open modes to integer oflag versions.
        OFLAGS = {
          "r"  => ::File::RDONLY,
          "r+" => ::File::RDWR   | ::File::CREAT,
          "w"  => ::File::WRONLY | ::File::CREAT  | ::File::TRUNC,
          "w+" => ::File::RDWR   | ::File::CREAT  | ::File::TRUNC,
          "a"  => ::File::WRONLY | ::File::APPEND | ::File::CREAT,
          "a+" => ::File::RDWR   | ::File::APPEND | ::File::CREAT
        }

        def process_options
          redirects = {}
          others = {}

          @options.each do |key, value|
            case key
              when ::Array
                key.each do |fd|
                  redirects[fd] = adjust_redirect_value(fd, value)
                end
              when :in, :out, :err, ::Fixnum, ::IO
                redirects[key] = adjust_redirect_value(key, value)
              when :pgroup
                if value == true
                  value = 0
                elsif value
                  value = Rubinius::Type.coerce_to value, ::Integer, :to_int
                  raise ArgumentError, "negative process group ID : #{value}" if value < 0
                end
                others[key] = value
              when :chdir
                others[key] = Rubinius::Type.coerce_to_path(value)
              when :unsetenv_others, :umask, :close_others
                others[key] = value
              when ::Symbol
                raise ArgumentError, "wrong exec option symbol : #{key.inspect}"
              else
                raise ArgumentError, "wrong exec option"
            end
          end

          @redirects = redirects
          @options = others
        end

        # Converts the various supported command argument variations into a
        # standard argv suitable for use with exec. This includes detecting commands
        # to be run through the shell.
        #
        # The args array may follow any of these variations:
        #
        # 'true'                     => ['true', ['true']]
        # 'echo', 'hello', 'world'   => ['echo', ['echo', 'hello', 'world']]
        # 'echo *'                   => ['/bin/sh', ['sh', '-c', 'echo *']]
        # ['echo', 'fuuu'], 'hello'  => ['echo', ['fuuu', 'hello']]
        def process_command
          if @arguments.empty? and
              cmd = Rubinius::Type.try_convert(@command, ::String, :to_str)
            if cmd.empty?
              raise Errno::ENOENT
            else
              @command = cmd
              @argv = []
              return
            end
          elsif cmd = Rubinius::Type.try_convert(@command, ::Array, :to_ary)
            raise ArgumentError, "wrong first argument" unless cmd.size == 2
            command = StringValue(cmd[0])
            name = StringValue(cmd[1])
          else
            name = command = StringValue(@command)
          end

          argv = [name]
          @arguments.each do |arg|
            argv << StringValue(arg)
          end

          @command = command
          @argv = argv
        end

        # Convert variations of redirecting to a file to a standard tuple.
        #
        # :in   => '/some/file'   => ['/some/file', 'r', 0644]
        # :out  => '/some/file'   => ['/some/file', 'w', 0644]
        # :err  => '/some/file'   => ['/some/file', 'w', 0644]
        # STDIN => '/some/file'   => ['/some/file', 'r', 0644]
        #
        # Returns the modified value.
        def adjust_redirect_value(key, value)
          case value
            when :in, :out, :err, :close, ::IO, ::Fixnum
              value
            when ::Array
              if value.size < 3
                defaults = default_file_reopen_info(key, value[0])
                value += defaults[value.size..-1]
              end

              if value[1].respond_to?(:to_str)
                value = value.dup
                value[1] = OFLAGS[value[1]]
              end

              value
            when ::String
              default_file_reopen_info(key, value)
            when ::Symbol
              raise ArgumentError, "wrong exec redirect symbol : #{value.inspect}"
            else
              raise ArgumentError, "wrong exec redirect action"
          end
        end

        # The default [file, flags, mode] tuple for a given fd and filename. The
        # default flags vary based on the what fd is being redirected. stdout and
        # stderr default to write, while stdin and all other fds default to read.
        #
        # fd   - The file descriptor that is being redirected. This may be an IO
        #        object, integer fd number, or :in, :out, :err for one of the standard
        #        streams.
        # file - The string path to the file that fd should be redirected to.
        #
        # Returns a [file, flags, mode] tuple.
        def default_file_reopen_info(fd, file)
          case fd
          when :in, ::STDIN, $stdin, 0
            [file, OFLAGS["r"], 0644]
          when :out, STDOUT, $stdout, 1
            [file, OFLAGS["w"], 0644]
          when :err, STDERR, $stderr, 2
            [file, OFLAGS["w"], 0644]
          else
            [file, OFLAGS["r"], 0644]
          end
        end

        # Convert a fd identifier to an IO object.
        #
        # Returns nil or an instance of IO.
        def fd_to_io(object)
          case object
          when ::IO, ::STDIN, ::STDOUT, ::STDERR, $stdin, $stdout, $stderr
            object
          when :in, 0
            ::STDIN
          when :out, 1
            ::STDOUT
          when :err, 2
            ::STDERR
          when ::Fixnum
            object >= 0 ? ::IO.for_fd(object) : nil
          else
            object.respond_to?(:to_io) ? object.to_io : nil
          end
        end

        def setup_redirects
          @redirects.each do |key, val|
            if val == :close
              fd_to_io(key).close_on_exec = true
            elsif val.is_a?(::Array)
              file, mode_string, perms = *val
              fd_to_io(key).reopen(::File.open(file, mode_string, perms))
            else
              val = fd_to_io(val)
              val.close_on_exec = false
              val.autoclose = false

              case key
              when ::Symbol
                key = fd_to_io(key)
              when ::Fixnum
                key = fd_to_io FFI::Platform::POSIX.dup2(val.fileno, key)
              end

              key.reopen(val)
              key.close_on_exec = false
              key.autoclose = false
            end
          end
        end

        def setup_options
          ENV.clear if @options[:unsetenv_others]

          if chdir = @options[:chdir]
            ::Dir.chdir(chdir)
          end

          # { :pgroup => pgid } options
          pgroup = @options[:pgroup]
          pgroup = 0 if pgroup == true
          ::Process.setpgid(0, pgroup) if pgroup

          if umask = @options[:umask]
            ::File.umask(umask)
          end

          unless @options[:close_others] == false
            3.upto(::IO.max_open_fd).each do |fd|
              begin
                ::IO.for_fd(fd, :autoclose => false).close_on_exec = true
              rescue Errno::EBADF
              end
            end
          end
        end

        def setup_process
          setup_options
          setup_redirects
          ENV.update @environment
        end

        def spawn(exe, command, args)
          Rubinius.primitive :vm_spawn
          raise PrimitiveFailure,
            "Rubinius::Mirror::Process::Execute#spawn primitive failed"
        end

        def exec(command, args)
          Rubinius.primitive :vm_exec
          raise PrimitiveFailure,
            "Rubinius::Mirror::Process::Execute#exec primitive failed"
        end
      end
    end
  end
end
