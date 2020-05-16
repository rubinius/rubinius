TOPLEVEL_BINDING = binding()

module Rubinius
  class Loader
    def initialize
      @exit_code    = 0
      @process_id   = Process.pid
      @load_paths   = []
      @requires     = []
      @evals        = []
      @script       = nil
      @debugging    = false
      @repl         = true
      @printed_version = false
      @input_loop   = false
      @input_loop_print = false
      @input_loop_split = false
      @simple_options = false
      @early_option_stop = false
      @check_syntax = false
      @load_profiler = false
      @enable_gems = true
      @load_gemfile = false
      @deprecations = false

      version = RUBY_VERSION.split(".").first(2).join(".")
      @gem_bins = ["#{version}/bin", "bin"].map do |dir|
        File.join Rubinius::GEMS_PATH, dir
      end
    end

    private :initialize

    def self.debugger
      @debugger_proc
    end

    def self.debugger=(prc)
      @debugger_proc = prc
    end

    # Finish setting up after loading the core library.
    def preamble
      set_program_name "rbx"
      @stage = "running Loader preamble"

      # Set the default visibility for the top level binding
      TOPLEVEL_BINDING.variables.method_visibility = :private

      $VERBOSE = false
    end

    # Set up $LOAD_PATH.
    def system_load_path
      @stage = "setting up system load path"

      @lib_bin = "#{Rubinius::CODEDB_PATH}/source/bin"

      $LOAD_PATH.unshift(Rubinius::ARCH_PATH,
                         Rubinius::SITE_PATH,
                         "#{Rubinius::SITE_PATH}/#{RUBY_VERSION}",
                         Rubinius::VENDOR_PATH,
                         "#{Rubinius::CODEDB_PATH}/source")

      if libs = ENV['RUBYLIB']
        Rubinius::Logger.system.write "RUBYLIB: #{libs}"

        $LOAD_PATH.unshift(*libs.split(File::PATH_SEPARATOR).compact)
      end
    end

    # Load customization code:
    #   /etc/rbxrc
    #   $HOME/.rbxrc
    #   $RBX_PRELOAD
    def preload
      @stage = "preloading rbxrc code"

      ['/etc/rbxrc',"#{ENV['HOME']}/.rbxrc",ENV['RBX_PRELOAD']].each do |file|
        begin
          load file if file and File.exist?(file)
        rescue LoadError
          nil
        end
      end
    end

    # Register signal handlers.
    def signals
      @stage = "registering signal handlers"

      # Set up a handler for SIGINT that raises Interrupt on the main thread
      Signal.trap("INT") do |sig|
        raise Interrupt, "Thread has been interrupted"
      end

      ["HUP", "QUIT", "TERM", "ALRM", "USR1", "USR2"].each do |signal|
        Signal.trap(signal) do |sig|
          raise SignalException, sig
        end
      end
    end

    def show_syntax_error(e)
      STDERR.puts "A syntax error has occurred:"
      STDERR.puts "    #{e.message}"
      STDERR.puts "\nCode:\n#{e.code}"
      if e.column
        STDERR.puts((" " * (e.column - 1)) + "^")
      end
    end

    def show_syntax_errors(syns)
      syns.each do |e|
        STDERR.puts "#{e.file}:#{e.line}: #{e.reason}"
      end
    end

    # Checks if a subcommand with basename +base+ exists. Returns the full
    # path to the subcommand if it does; otherwise, returns nil.
    def find_subcommand(base)
      command = File.join @lib_bin, "#{base}.rb"
      return command if File.exist? command
      return nil
    end

    # Checks if a gem wrapper named +base+ exists. Returns the full path to
    # the gem wrapper if it does; otherwise, returns nil.
    def find_gem_wrapper(base)
      @gem_bins.each do |dir|
        wrapper = File.join dir, base
        return wrapper if File.exist? wrapper
      end
      return nil
    end

    # Detects if the Rubinius executable was aliased to a subcommand or a
    # rubygems executable. If so, changes ARGV as if Rubinius were invoked
    # to run the subcommand or rubygems executable.
    def detect_alias
      cmd = ARG0.split("/").last

      # ignore the common ones straight away
      return if cmd == "rbx" or cmd == "ruby"

      # Check if we are aliased to a Rubinius subcommand.
      if subcommand = find_subcommand(cmd)
        ARGV.unshift subcommand
        return
      end

      # Check if we are aliased to a rubygems executable.
      if gem_wrapper = find_gem_wrapper(cmd)
        ARGV.unshift "-S", gem_wrapper
      end
    end

    def handle_simple_options(argv)
      argv.delete_if do |x|
        if x[0] == ?-
          if equal = x.index("=")
            name = x.substring(1, equal-1)

            equal += 1
            val = x.substring(equal, x.size - equal)
          else
            name = x.substring(1, x.size - 1)
            val = true
          end

          name.gsub! "-", "_"

          Rubinius::Globals[:"$#{name}"] = val

          true
        else
          false
        end
      end
    end

    def define_global_methods
      Kernel.module_eval do
        def gsub(pattern, rep=nil, &block)
          target = $_
          raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
          $_ = target.gsub(pattern, rep, &block)
        end
        module_function :gsub

        def sub(pattern, rep=nil, &block)
          target = $_
          raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
          $_ = target.sub(pattern, rep, &block)
        end
        module_function :sub

        def chomp(string=$/)
          raise TypeError, "$_ must be a String" unless $_.kind_of? String
          $_ = $_.chomp(string)
        end
        module_function :chomp

        def chop
          raise TypeError, "$_ must be a String" unless $_.kind_of? String
          $_ = $_.chop
        end
        module_function :chop
      end
    end

    # Process all command line arguments.
    def options(argv=ARGV)
      @stage = "processing command line arguments"

      options = Options.new "Usage: rbx [subcommand] [options] [--] [script] [arguments]", 25

      options.left_align
      options.on_extra do |x|
        raise Options::ParseError, "Unrecognized option: #{x}" if x[0] == ?-
        if @script.nil? and @evals.empty?
          @script = x
        else
          argv.unshift x
        end
        options.stop_parsing
      end

      options.doc "Script is any valid Ruby source code file"

      options.doc "\nRuby options"
      options.on "-", "Read and evaluate code from STDIN" do
        @repl = false
        set_program_name "-"
        stdin = STDIN.dup
        script = STDIN.read
        STDIN.reopen(stdin)
        execute_script script
      end

      options.on "--", "Stop processing command line arguments" do
        @early_option_stop = true
        options.stop_parsing
      end

      options.on "-a", "Used with -n and -p, splits $_ into $F" do
        @input_loop_split = true
        Rubinius::Globals.set!(:$-a, true)
      end

      options.on "-c", "Only check the syntax" do
        @repl = false
        @check_syntax = true
      end

      options.on "-C", "DIR", "Change directory to DIR before running scripts" do |dir|
        @directory = dir
      end

      options.on "-d", "Enable debugging output and set $DEBUG to true" do
        $VERBOSE = true
        $DEBUG = true
      end

      options.on "--disable-gems", "Do not automatically load rubygems on startup" do
        @enable_gems = false
      end

      options.on "-e", "CODE", "Compile and execute CODE" do |code|
        @repl = false
        set_program_name "(eval)"
        @evals << code
      end

      options.on '-F', "PATTERN", "Set $; to PATTERN" do |pattern|
        Rubinius::Globals.set!(:$;, Regexp.new(pattern))
      end

      options.on '-G', '--gemfile', 'Respect a Gemfile in the current path' do
        @load_gemfile = true
      end

      options.on "-h", "--help", "Display this help" do
        @repl = false
        puts options
        done
      end

      options.on "-i", "[EXT]", "Edit ARGV files in place, making backup with EXT" do |ext|
        # in place edit mode
        ext ||= ''
        $-i = ext
      end

      options.on "-I", "DIR1[:DIR2]", "Add directories to $LOAD_PATH" do |dir|
        @load_paths << dir
      end

      options.on "-K", "Ignored $KCODE option for compatibility" do
        msg = "The -K option is deprecated. Rubinius internal encoding is always UTF-8"
        STDERR.puts msg
        Rubinius::Logger.system.warn msg
      end

      options.on "-U", "Set Encoding.default_internal to UTF-8" do
        msg = "The -U option is deprecated. Rubinius internal encoding is always UTF-8"
        STDERR.puts msg
        Rubinius::Logger.system.warn msg
      end

      options.on "-E", "ENC", "Set external:internal character encoding to ENC" do |enc|
        external, internal = enc.split(":")
        Encoding.default_external = external if external and !external.empty?

        if internal and !internal.empty?
          msg = "The -E option setting internal encoding is deprecated. Rubinius internal encoding is always UTF-8"
          STDERR.puts msg
          Rubinius::Logger.system.warn msg
        end
      end

      options.on "--main", "PATH", "Load PATH directly from CodeDB" do |path|
        code = Rubinius::CoreDB.load_feature path, "", false, false
        if code
          code.create_script false
          Rubinius.run_script code
          exit 0
        else
          puts "file not found: #{path}"
          exit 1
        end
      end

      options.on "-n", "Wrap running code in 'while(gets()) ...'" do
        @input_loop = true
        define_global_methods
      end

      options.on "-p", "Same as -n, but also print $_" do
        @input_loop = true
        @input_loop_print = true
        Rubinius::Globals.set!(:$-p, true)
        define_global_methods
      end

      options.on "-r", "LIBRARY", "Require library before execution" do |file|
        @requires << file
      end

      options.on("-s", "Process options after the script into globals") do
        @simple_options = true
      end

      options.on("-S", "SCRIPT",
                 "Run SCRIPT using PATH environment variable to find it") do |script|
        options.stop_parsing
        @repl = false

        # Load a gemfile now if we need to so that -S can see binstubs
        # internal to the Gemfile
        gemfile

        # First, check if any existing gem wrappers match.
        unless file = find_gem_wrapper(script)
          # Then, check if any Rubinius subcommands match.
          unless file = find_subcommand(script)
            # Finally, check if any file on PATH matches.
            search = ENV['PATH'].split(File::PATH_SEPARATOR)
            search.each do |d|
              path = File.join d, script
              if File.exist? path
                file = path
                break
              end
            end
          end
        end

        set_program_name script if file

        # if missing, let it die a natural death
        @script = file ? file : script
      end

      options.on "-v", "Display the version and set $VERBOSE to true" do
        @repl = false
        $VERBOSE = true

        unless @printed_version
          puts Rubinius.version
          @printed_version = true
        end
      end

      options.on "-w", "Enable warnings" do
        $VERBOSE = true
      end

      options.on("-W", "[level]",
                 "Set warning level: 0=silence, 1=medium, 2=verbose (default)") do |l|
        case l
        when "0"
          $VERBOSE = nil
        when "1"
          $VERBOSE = false
        when nil
          $VERBOSE = true
        else
          # MRI -h says -W2 sets $VERBOSE to true, but behaviorally
          # any value >= 2 sets $VERBOSE to true.
          $VERBOSE = true
        end
      end

      options.on "--show-nil-location", "Show where nil value originated when NoMethodError on nil is raised" do
        $rbx_show_nil_location = true
      end

      options.on "--deprecated", "Display any deprecations" do
        @deprecations = true
      end

      options.on "--version", "Display the version" do
        @repl = false
        puts Rubinius.version
      end

      options.doc <<-DOC
\nRubinius subcommands

    Rubinius provides subcommands that implement various facilities. Each
    subcommand provides its own help. For example:

      rbx compile -h

    Available subcommands:

      compile       Run the bytecode compiler
      gem           Run RubyGems 'gem' command
      irb           Run IRB
      erb           Run ERb
    DOC

      options.doc <<-DOC
VM Options
   -X<variable>[=<value>]
     This option is recognized by the VM before any ruby code is loaded.
     It is used to set VM configuration options.

     Use -Xhelp to see the list of options the VM recognizes.
     All variables, even ones that the VM doesn't understand, are available
     in Rubinius::Config.

     Options are parsed from these sources, in this order:

     1. The file .rbxrc in the current working directory.
     2. The RBXOPT environment variable.
     3. The command line options.

     This order permits environment and command line options to override
     "application" configuration. Likewise, command line options can override
     environment configuration.

     A number of Rubinius features are driven by setting these variables.
      DOC

      options.parse ARGV

      handle_rubyopt(options)

      if @early_option_stop and @simple_options
        handle_simple_options(argv)
      end

      if Rubinius::Config['help']
        STDOUT.puts "Rubinius configuration variables:"
        STDOUT.puts "  These variables are normally set via -X and read via Rubinius::Config[var]."
        STDOUT.puts

        require 'rubinius/configuration'
        Rubinius::ConfigurationVariables.instance.show_help(STDOUT)
        exit 0
      end

      exit 0 if Rubinius::Config["config.print"]

      if str = Rubinius::Config['tool.require']
        begin
          require str
        rescue LoadError
          STDERR.puts "Unable to require file for tool: '#{str}'"
        end
      end

      if Rubinius::Config['profile'] || Rubinius::Config['jit.profile']
        @load_profiler = true
      end

      if @check_syntax
        check_syntax
      end
    end

    # Sets $0 ($PROGRAM_NAME) without changing the process title
    def set_program_name(name)
      Rubinius::Globals.set! :$0, name
    end
    private :set_program_name

    def handle_rubyopt(options)
      if env_opts = ENV['RUBYOPT']
        Rubinius::Logger.system.write "RUBYOPT: #{env_opts}"

        options.start_parsing
        env_opts = env_opts.strip.split(/\s+/)

        until env_opts.empty?
          entry = env_opts.shift

          unless entry[0] == ?-
            entry = "-#{entry}"
          end

          opt, arg, rest = options.split entry, 2

          options.process env_opts, entry, opt, arg
        end
      end

    end

    # Update the load paths with any -I arguments.
    def load_paths
      @stage = "setting load paths"

      @load_paths.each do |path|
        path.split(":").reverse_each do |path|
          # We used to run expand_path on path first, but MRI
          # doesn't and it breaks some code if we do.
          $LOAD_PATH.unshift(path)
        end
      end
    end

    def deprecations
      @stage = "showing deprecations"

      return unless @deprecations

      if Rubinius::DEPRECATIONS.size > 0
        Rubinius::DEPRECATIONS.each do |desc, alt|
          STDERR.puts <<-EOM
Deprecation: #{desc}
Alternative: #{alt}

          EOM
        end

        exit 1
      else
        STDERR.puts "Rubinius #{Rubinius::VERSION} has no deprecation notices"
        exit 0
      end
    end

    def load_compiler
      @stage = "loading the compiler"

      begin
        CodeLoader.load_compiler
      rescue LoadError => e
        message = "Unable to load the bytecode compiler."
        Rubinius::Logger.system.error message
        Rubinius::Logger.system.error(
          "Please run 'rake' or 'rake install' to rebuild the compiler.")
        Rubinius::Logger.log_exception message, e
        exit 1
      end
    end

    def debugger
      @stage = "running the debugger"

      if Rubinius::Config['debug']
        if custom = Loader.debugger
          custom.call
        else
          require 'rubinius/debugger'
          Rubinius::Debugger.start
        end
      end
    end

    def rubygems
      @stage = "loading Rubygems"

      CodeLoader.load_rubygems if @enable_gems
    end

    def gemfile
      @stage = "loading Gemfile"

      if @load_gemfile
        CodeLoader.load_rubygems
        require 'bundler/setup'
        @load_gemfile = false
      end
    end

    def profiler
      return unless @load_profiler
      @stage = "loading profiler"

      require 'profile'
    end

    # Require any -r arguments
    def requires
      @stage = "requiring command line files"

      @requires.each { |file| require file }
    end

    # Evaluate any -e arguments
    def evals
      return if @evals.empty?

      @repl = false
      @stage = "evaluating command line code"

      Dir.chdir(@directory) if @directory

      if @input_loop
        while gets
          $F = $_.split if @input_loop_split
          eval @evals.join("\n"), TOPLEVEL_BINDING, "-e", 1
          print $_ if @input_loop_print
        end
      else
        eval @evals.join("\n"), TOPLEVEL_BINDING, "-e", 1
      end
    end

    # Run the script passed on the command line
    def script
      return unless @script and @evals.empty?

      @repl = false

      handle_simple_options(ARGV) if @simple_options

      @stage = "running #{@script}"
      Dir.chdir @directory if @directory

      if File.exist? @script
        if IO.read(@script, 6) == "!RBIX\n"
          raise LoadError, "'#{@script}' is not a Ruby source file"
        end
      else
        if @script.suffix?(".rb")
          raise LoadError, "unable to find '#{@script}'"
        else
          command = File.join @lib_bin, "#{@script}.rb"
          unless File.exist? command
            raise LoadError, "unable to find Rubinius command '#{@script}'"
          else
            @script = command
          end
        end
      end

      set_program_name @script
      CodeLoader.new(@script).load_script
    end

    #Check Ruby syntax of source
    def check_syntax
      parser_class = Rubinius::ToolSets::Runtime::Melbourne

      if @script
        if File.exist?(@script)
          parser = parser_class.new @script, 1, []
          parser.parse_file
        else
          puts "rbx: Unable to find file -- #{@script} (LoadError)"
          exit 1
        end
      elsif not @evals.empty?
        parser = parser_class.new('-e', 1, [])
        parser.parse_string(@evals.join("\n"))
      else
        parser = parser_class.new('-', 1, [])
        parser.parse_string(STDIN.read)
      end

      puts "Syntax OK"
      exit 0
    rescue SyntaxError => e
      show_syntax_errors(parser.syntax_errors)
      exit 1
    end

    # Run IRB unless we were passed -e, -S arguments or a script to run.
    def repl
      return unless @repl

      @stage = "running IRB"

      # Check if a different REPL is set.
      repl = ENV["RBX_REPL"] || "irb"

      if Terminal
        load File.join(Rubinius::GEMS_PATH, "bin", repl)
      else
        set_program_name "(eval)"
        execute_script "p #{STDIN.read}"
      end
    end

		def execute_script(script)
			eval script, TOPLEVEL_BINDING
		end

    def run_at_exits
      until AtExit.empty?
        begin
          AtExit.shift.call
        rescue SystemExit => e
          @exit_code = e.status
          # We recurse down so that future at_exits
          # see the current exception
          run_at_exits
        end
      end
    end

    def flush_stdio
      STDOUT.flush unless STDOUT.closed?
      STDERR.flush unless STDERR.closed?
    end

    def exit_with_exception(e)
      @exit_code = 1
      Rubinius::Logger.log_exception "An exception occurred #{@stage}", e
    end

    # Cleanup and at_exit processing.
    def epilogue
      @stage = "running at_exit handlers"

      begin
        Signal.run_handler Signal::Names["EXIT"]
      rescue SystemExit => e
        @exit_code = e.status
      end

      run_at_exits

      flush_stdio

    rescue Object => e
      exit_with_exception e
    end

    # Exit.
    def done
      # check that this is a valid exit rather than failing to process
      # unwinding properly.
      #
      # TODO: this code is pretty gross, nice object inspectors, please.
      thread_state = Rubinius.thread_state
      reason = thread_state[0]
      unless reason == :none
        STDERR.puts "\nERROR: the VM is exiting improperly #{@stage}"
        STDERR.puts "intended operation: #{reason.inspect}"
        STDERR.puts "associated value: #{thread_state[1].inspect}"

        destination = thread_state[2]

        if destination
          method = destination.method

          STDERR.puts "destination scope:"
          STDERR.puts "  method: #{method.name} at #{method.file}:#{method.first_line}"
          STDERR.puts "  module: #{destination.module.name}"
          STDERR.puts "  block:  #{destination.block}" if destination.block
        else
          STDERR.puts "destination scope: unknown"
        end

        if reason == :catch_throw
          STDERR.puts "throw destination: #{thread_state[4].inspect}"
        end

        if exception = thread_state[3]
          begin
            exception.render
          rescue Exception => e
            STDERR.puts "Unable to render backtrace: #{e.message} (#{e.class})"
            STDERR.puts "Raw backtrace data:"

            exception.locations.each do |loc|
              p [loc.file, loc.line]
            end
          end
        end

        @exit_code = 1
      end

      Process.exit! @exit_code
    end

    def handle_exception(e)
      case e
      when SystemExit
        @exit_code = e.status
      when SyntaxError
        @exit_code = 1

        STDERR.puts e.awesome_backtrace.show
        STDERR.puts

        show_syntax_error(e)
      when Interrupt
        exit_with_exception e
      when SignalException
        Signal.trap(e.signo, "SIG_DFL")
        Process.kill e.signo, Process.pid
      when nil
        # what?
      else
        exit_with_exception e
      end
    rescue Object => e
      exit_with_exception e
    ensure
      epilogue
    end

    # Orchestrate everything.
    def main
      preamble
      system_load_path
      signals
      load_compiler
      preload
      detect_alias
      options
      load_paths
      deprecations
      rubygems
      gemfile
      debugger
      profiler
      requires
      evals
      script
      repl

    rescue Object => e
      handle_exception e
    else
      # We do this, run epilogue both in the rescue blocks and also here,
      # so that at_exit{} hooks can read $!.
      epilogue
    ensure
      done
    end
  end
end

