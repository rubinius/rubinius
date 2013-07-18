# -*- encoding: us-ascii -*-

TOPLEVEL_BINDING = binding()

# Default kcode
Rubinius.kcode = "ASCII"

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
      @run_irb      = true
      @printed_version = false
      @input_loop   = false
      @input_loop_print = false
      @input_loop_split = false
      @simple_options = false
      @early_option_stop = false
      @check_syntax = false

      @enable_gems = !Rubinius.ruby18?
      @load_gemfile = false

      version = RUBY_VERSION.split(".").first(2).join(".")
      @gem_bins = ["#{version}/bin", "bin"].map do |dir|
        File.join Rubinius::GEMS_PATH, dir
      end
    end

    def self.debugger
      @debugger_proc
    end

    def self.debugger=(prc)
      @debugger_proc = prc
    end

    # Finish setting up after loading kernel.
    def preamble
      set_program_name "rbx"
      @stage = "running Loader preamble"

      Object.const_set :ENV, EnvironmentVariables.new

      # Set the default visibility for the top level binding
      TOPLEVEL_BINDING.variables.method_visibility = :private

      $VERBOSE = false
    end

    # Setup $LOAD_PATH.
    def system_load_path
      @stage = "setting up system load path"

      @main_lib = Rubinius::LIB_PATH

      @main_lib_bin = File.join @main_lib, "bin"
      Rubinius.const_set :PARSER_EXT_PATH, "#{@main_lib}/ext/melbourne/rbx/melbourne20"

      # This conforms more closely to MRI. It is necessary to support
      # paths that mkmf adds when compiling and installing native exts.
      additions = [
        Rubinius::SITE_PATH,
        "#{Rubinius::SITE_PATH}/#{Rubinius::CPU}-#{Rubinius::OS}",
        Rubinius::VENDOR_PATH,
        "#{Rubinius::VENDOR_PATH}/#{Rubinius::CPU}-#{Rubinius::OS}",
        "#{@main_lib}/#{Rubinius::RUBY_LIB_VERSION}",
        @main_lib,
      ]
      additions.uniq!

      $LOAD_PATH.unshift(*additions)

      if ENV['RUBYLIB'] and not ENV['RUBYLIB'].empty? then
        rubylib_paths = ENV['RUBYLIB'].split(File::PATH_SEPARATOR)
        $LOAD_PATH.unshift(*rubylib_paths)
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
      command = File.join @main_lib_bin, "#{base}.rb"
      return command if File.exists? command
      return nil
    end

    # Checks if a gem wrapper named +base+ exists. Returns the full path to
    # the gem wrapper if it does; otherwise, returns nil.
    def find_gem_wrapper(base)
      @gem_bins.each do |dir|
        wrapper = File.join dir, base
        return wrapper if File.exists? wrapper
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
        @run_irb = false
        set_program_name "-"
        CodeLoader.execute_script STDIN.read
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
        @run_irb = false
        @check_syntax = true
      end

      options.on "-C", "DIR", "Change directory to DIR before running scripts" do |dir|
        @directory = dir
      end

      options.on "-d", "Enable debugging output and set $DEBUG to true" do
        $VERBOSE = true
        $DEBUG = true
      end

      unless Rubinius.ruby18?
        options.on "--disable-gems", "Do not automatically load rubygems on startup" do
          @enable_gems = false
        end
      end

      options.on "-e", "CODE", "Compile and execute CODE" do |code|
        @run_irb = false
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
        @run_irb = false
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

      if Rubinius.ruby18?
        options.on "-K", "[code]", "Set $KCODE" do |k|
          case k
          when 'a', 'A', 'n', 'N', nil
            $KCODE = "NONE"
          when 'e', 'E'
            $KCODE = "EUC"
          when 's', 'S'
            $KCODE = "SJIS"
          when 'u', 'U'
            $KCODE = "UTF8"
          else
            $KCODE = "NONE"
          end
        end
      else
        options.on "-K", "Ignored $KCODE option for compatibility"
        options.on "-U", "Set Encoding.default_internal to UTF-8" do
          set_default_internal_encoding('UTF-8')
        end

        options.on "-E", "ENC", "Set external:internal character encoding to ENC" do |enc|
          ext, int = enc.split(":")
          Encoding.default_external = ext if ext and !ext.empty?
          set_default_internal_encoding(int) if int and !int.empty?
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
        @run_irb = false

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
              if File.exists? path
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
        @run_irb = false
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

      options.on "--version", "Display the version" do
        @run_irb = false
        puts Rubinius.version
      end

      options.doc <<-DOC
\nRubinius subcommands

    Rubinius provides subcommands that implement various facilities. Each
    subcommand provides its own help. For example:

      rbx compile -h

    Available subcommands:

      compile       Run the bytecode compiler
      console       Run the Agent CLI
      docs          Run a local HTTP documentation server
      gem           Run RubyGems 'gem' command
      report        Create a gist of the last Rubinius toplevel exception
    DOC

      options.doc <<-DOC
VM Options
   -X<variable>[=<value>]
     This option is recognized by the VM before any ruby code is loaded.
     It is used to set VM configuration options.

     Use -Xconfig.print to see the list of options the VM recognizes.
     All variables, even ones that the VM doesn't understand, are available
     in Rubinius::Config.

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
        require 'profile'
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

    def set_default_internal_encoding(encoding)
      if @default_internal_encoding_set && Encoding.default_internal.name != encoding
        raise RuntimeError, "Default internal encoding already set to '#{Encoding.default_internal.name}'."
      else
        @default_internal_encoding_set = true
        Encoding.default_internal = encoding
      end
    end
    private :set_default_internal_encoding

    RUBYOPT_VALID_OPTIONS = "IdvwWrKT"

    def handle_rubyopt(options)
      if ENV['RUBYOPT']
        options.start_parsing
        env_opts = ENV['RUBYOPT'].strip.split(/\s+/)

        until env_opts.empty?
          entry = env_opts.shift

          unless entry[0] == ?-
            entry = "-#{entry}"
          end

          opt, arg, rest = options.split entry, 2

          unless RUBYOPT_VALID_OPTIONS.index opt[1, 1]
            raise RuntimeError, "invalid option in RUBYOPT: #{opt}"
          end

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

    def load_compiler
      @stage = "loading the compiler"

      begin
        CodeLoader.load_compiler
      rescue LoadError => e
        STDERR.puts <<-EOM
Unable to load the bytecode compiler. Please run 'rake' or 'rake install'
to rebuild the compiler.

        EOM

        e.render
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

      require "rubygems" if @enable_gems
    end

    def gemfile
      @stage = "loading Gemfile"

      if @load_gemfile
        require 'rubygems'
        require 'bundler/setup'
        @load_gemfile = false
      end
    end

    # Require any -r arguments
    def requires
      @stage = "requiring command line files"

      @requires.each { |file| require file }
    end

    # Evaluate any -e arguments
    def evals
      return if @evals.empty?

      @run_irb = false
      @stage = "evaluating command line code"

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

      @run_irb = false

      handle_simple_options(ARGV) if @simple_options

      @stage = "running #{@script}"
      Dir.chdir @directory if @directory

      if File.exists? @script
        if IO.read(@script, 6) == "!RBIX\n"
          raise LoadError, "'#{@script}' is not a Ruby source file"
        end
      else
        if @script.suffix?(".rb")
          raise LoadError, "unable to find '#{@script}'"
        else
          command = File.join @main_lib_bin, "#{@script}.rb"
          unless File.exists? command
            raise LoadError, "unable to find Rubinius command '#{@script}'"
          else
            @script = command
          end
        end
      end

      set_program_name @script
      CodeLoader.load_script @script, @debugging
    end

    #Check Ruby syntax of source
    def check_syntax
      syntax_ok = false

      case
      when Rubinius.ruby18?
        parser = Rubinius::Melbourne
      when Rubinius.ruby19?
        parser = Rubinius::Melbourne19
      when Rubinius.ruby20?
        parser = Rubinius::Melbourne20
      else
        raise "no parser available for this ruby version"
      end

      if @script
        if File.exists?(@script)
          mel = parser.new @script, 1, []

          begin
            mel.parse_file
          rescue SyntaxError => e
            show_syntax_errors(mel.syntax_errors)
            exit 1
          end
        else
          puts "rbx: Unable to find file -- #{@script} (LoadError)"
          exit 1
        end
      elsif not @evals.empty?
        begin
          mel = parser.parse_string @evals.join("\n")
        rescue SyntaxError => e
          show_syntax_errors(mel.syntax_errors)
          exit 1
        end
      else
        begin
          mel = parser.parse_string STDIN.read
        rescue SyntaxError => e
          show_syntax_errors(mel.syntax_errors)
          exit 1
        end
      end

      puts "Syntax OK"
      exit 0
    end

    # Run IRB unless we were passed -e, -S arguments or a script to run.
    def irb
      return unless @run_irb

      @stage = "running IRB"

      if Terminal
        repr = ENV['RBX_REPR'] || "bin/irb"
        set_program_name repr
        prog = File.join @main_lib, repr
        begin
          # HACK: this was load but load raises LoadError
          # with prog == "lib/bin/irb". However, require works.
          # Investigate when we have specs running.
          require prog
        rescue LoadError => e
          STDERR.puts "Unable to load REPL named '#{repr}'"
          STDERR.puts e.message
          puts e.awesome_backtrace.show
          exit 1
        end
      else
        set_program_name "(eval)"
        CodeLoader.execute_script "p #{STDIN.read}"
      end
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

    # Cleanup and at_exit processing.
    def epilogue
      @stage = "running at_exit handlers"

      begin
        Signal.run_handler Signal::Names["EXIT"]
      rescue SystemExit => e
        @exit_code = e.status
      end

      run_at_exits

      @stage = "running object finalizers"
      ::GC.start
      ObjectSpace.run_finalizers

      # TODO: Fix these with better -X processing
      if Config['rbx.jit_stats']
        p VM.jit_info
      end

    rescue Object => e
      e.render "An exception occurred #{@stage}"
      @exit_code = 1
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

    def write_last_error(e)
      unless path = Config['vm.crash_report_path']
        path = "#{ENV['HOME']}/.rbx/rubinius_last_error_#{@process_id}"
      end

      File.open(path, "wb") do |f|
        f.puts "Rubinius Crash Report #rbxcrashreport"
        f.puts ""
        f.puts "[[Exception]]"
        e.render("A toplevel exception occurred", f, false)

        f.puts ""
        f.puts "[[Version]]"
        f.puts Rubinius.version
      end
    rescue SystemCallError
      # Ignore writing the last error report
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
      debugger
      rubygems
      gemfile
      requires
      evals
      script
      irb

    rescue SystemExit => e
      @exit_code = e.status

      epilogue
    rescue SyntaxError => e
      @exit_code = 1

      show_syntax_error(e)

      STDERR.puts "\nBacktrace:"
      STDERR.puts e.awesome_backtrace.show
      epilogue
    rescue Interrupt => e
      @exit_code = 1

      write_last_error(e)
      e.render "An exception occurred #{@stage}"
      epilogue
    rescue SignalException => e
      Signal.trap(e.signo, "SIG_DFL")
      Process.kill e.signo, Process.pid
      epilogue
    rescue Object => e
      @exit_code = 1

      write_last_error(e)
      e.render "An exception occurred #{@stage}"
      epilogue
    else
      # We do this, run epilogue both in the rescue blocks and also here,
      # so that at_exit{} hooks can read $!.
      epilogue
    ensure
      done
    end

    # Creates an instance of the Loader and runs it. We catch any uncaught
    # exceptions here and report them before exiting.
    def self.main
      begin
        new.main
      rescue Object => exc
        STDERR.puts "\n====================================="
        STDERR.puts "Exception occurred during top-level exception output! (THIS IS BAD)"
        STDERR.puts
        STDERR.puts "Exception: #{exc.inspect} (#{exc.class})"
      end
    end
  end
end

