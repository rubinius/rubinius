TOPLEVEL_BINDING = binding()

# Default kcode
Rubinius.kcode = "ASCII"

module Rubinius
  class Loader
    def initialize
      @exit_code    = 0
      @load_paths   = []
      @requires     = []
      @evals        = []
      @script       = nil
      @verbose_eval = false
      @debugging    = false
      @run_irb      = true
      @printed_version = false

      @gem_bin = File.join Rubinius::GEMS_PATH, "bin"
    end

    # Finish setting up after loading kernel.
    def preamble
      @stage = "running Loader preamble"

      Object.const_set :ENV, EnvironmentVariables.new

      # define a global "start time" to use for process calculation
      $STARTUP_TIME = Time.now

      # set terminal width
      width = 80
      if Terminal and !ENV['RBX_NO_COLS']
        begin
          `which tput &> /dev/null`
          if $?.exitstatus == 0
            width = `tput cols`.to_i
          end
        end
      end
      Rubinius.const_set 'TERMINAL_WIDTH', width

      $VERBOSE = false
    end

    # Setup $LOAD_PATH.
    def system_load_path
      @stage = "setting up system load path"

      @main_lib = Rubinius::LIB_PATH

      unless File.exists? @main_lib
        if ENV['RBX_LIB']
          @main_lib = ENV['RBX_LIB']
        else
          STDERR.puts <<-EOM
Rubinius was configured to find standard library files at:

  #{@main_lib}

but that directory does not exist.

Set the environment variable RBX_LIB to the directory
containing the Rubinius standard library files.
          EOM
        end
      end

      # This conforms more closely to MRI. It is necessary to support
      # paths that mkmf adds when compiling and installing native exts.
      additions = []
      additions << Rubinius::SITE_PATH
      additions << "#{Rubinius::SITE_PATH}/#{Rubinius::CPU}-#{Rubinius::OS}"
      additions << Rubinius::VENDOR_PATH
      additions << "#{Rubinius::VENDOR_PATH}/#{Rubinius::CPU}-#{Rubinius::OS}"
      additions << @main_lib
      additions.uniq!

      $LOAD_PATH.unshift(*additions)

      if ENV['RUBYLIB'] and not ENV['RUBYLIB'].empty? then
        rubylib_paths = ENV['RUBYLIB'].split(':')
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
    end

    def show_syntax_error(e)
      STDERR.puts "A syntax error has occurred:"
      STDERR.puts "    #{e.message}"
      STDERR.puts "    near line #{e.file}:#{e.line}, column #{e.column}"
      STDERR.puts "\nCode:\n#{e.code}"
      if e.column
        STDERR.puts((" " * (e.column - 1)) + "^")
      end
    end

    # Process all command line arguments.
    def options(argv=ARGV)
      @stage = "processing command line arguments"

      options = Options.new "Usage: rbx [options] [--] [script] [arguments]", 25

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

      options.doc "Script is any valid Ruby source file (.rb) or a compiled Ruby file (.rbc)."

      options.doc "\nRuby options"
      options.on "-", "Read and evaluate code from STDIN" do
        @run_irb = false
        $0 = "-"
        CodeLoader.execute_script STDIN.read
      end

      options.on "--", "Stop processing command line arguments" do
        options.stop_parsing
      end

      options.on "-c", "FILE", "Check the syntax of FILE" do |file|
        mel = Rubinius::Melbourne.new file, 1, []
        begin
          mel.parse_file
        rescue SyntaxError => e
          show_syntax_error(e)
          exit 1
        end

        puts "Syntax OK"
        exit 0
      end

      options.on "-C", "DIR", "Change directory to DIR before running scripts" do |dir|
        @directory = dir
      end

      options.on "-d", "Enable debugging output and set $DEBUG to true" do
        $DEBUG = true
      end

      options.on "-e", "CODE", "Compile and execute CODE" do |code|
        @run_irb = false
        $0 = "(eval)"
        @evals << code
      end

      # Sexp not generated currently. --rue
      options.on "-E", "CODE", "Compile, show bytecode and execute CODE" do |code|
        @run_irb = false
        $0 = "(eval)"
        @verbose_eval = true
        @evals << code
      end

      options.on "-h", "--help", "Display this help" do
        @run_irb = false
        puts options
        done
      end

      options.on "-i", "EXT", "Edit ARGV files in place, making backup with EXT" do |ext|
        # in place edit mode
        $-i = ext
      end

      options.on "-I", "DIR1[:DIR2]", "Add directories to $LOAD_PATH" do |dir|
        @load_paths << dir
      end

      options.on "-r", "LIBRARY", "Require library before execution" do |file|
        @requires << file
      end

      options.on("-S", "SCRIPT",
                 "Run SCRIPT using PATH environment variable to find it") do |script|
        options.stop_parsing
        @run_irb = false

        search = [BIN_PATH, @gem_bin] + ENV['PATH'].split(File::PATH_SEPARATOR)
        dir    = search.detect do |d|
          path = File.join(d, script)
          File.exist?(path)
        end

        file = File.join(dir, script) if dir

        $0 = script if file

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

      # TODO: convert all these to -X options
      options.doc "\nRubinius options"

      options.on "--dc", "Display debugging information for the compiler" do
        puts "[Compiler debugging enabled]"
        $DEBUG_COMPILER = true
      end

      options.on "--dl", "Display debugging information for the loader" do
        $DEBUG_LOADING = true
        puts "[Code loading debugging enabled]"
      end

      options.on "--gc-stats", "Show GC stats" do
        stats = Stats::GC.new
        at_exit { stats.show }
      end

      @profile = Rubinius::Config['profile']

      options.on "-P", "Run the profiler" do
        puts "[WARN] -P is deprecated, please use -Xprofile"
        @profile = true
      end

      options.on "--vv", "Display version and extra info" do
        @run_irb = false

        $VERBOSE = true
        puts Rubinius.version
        puts "Options:"
        puts "  Interpreter type: #{INTERPRETER}"
        if jit = JIT
          puts "  JIT enabled: #{jit.join(', ')}"
        else
          puts "  JIT disabled"
        end
        puts
      end

      # This will only trigger if it's not the first option, in which case
      # we'll just tell the user to make it the first option.
      options.on "--rebuild-compiler", "Rebuild the Rubinius compiler" do
        puts "This must be the first and only option."
        exit 1
      end

      options.doc <<-DOC
\nVM Options
   -X<variable>[=<value>]
     This option is recognized by the VM before any ruby code loaded.
     It is used to set VM configuration options.

     Use -Xconfig.print to see the list of options the VM recognizes.
     All variables, even ones that VM doesn't understand, are available
     in Rubinius::Config.

     A number of Rubinius features are driven by setting these variables.
      DOC

      options.parse ARGV

      handle_rubyopt(options)

      if @profile
        require 'profile'
      end
    end

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

          options.process env_opts, entry, opt, arg
        end
      end

    end

    # Update the load paths with any -I arguments.
    def load_paths
      @stage = "setting load paths"

      @load_paths.each do |path|
        path.split(":").reverse_each do |path|
          path = File.expand_path path
          $LOAD_PATH.unshift(path)
        end
      end
    end

    def load_compiler
      @stage = "loading the compiler"

      # This happens before we parse ARGV, so we have to check ARGV ourselves
      # here.

      rebuild = (ARGV.last == "--rebuild-compiler")

      begin
        CodeLoader.require_compiled "compiler", rebuild ? false : true
      rescue Rubinius::InvalidRBC => e
        STDERR.puts "There was an error loading the compiler."
        STDERR.puts "It appears that your compiler is out of date with the VM."
        STDERR.puts "\nPlease use 'rbx --rebuild-compiler' or 'rake [instal]' to"
        STDERR.puts "bring the compiler back to date."
        exit 1
      end

      if rebuild
        STDOUT.puts "Rebuilding compiler..."
        files =
          ["#{@main_lib}/compiler.rb"] +
          Dir["#{@main_lib}/compiler/*.rb"] +
          Dir["#{@main_lib}/compiler/**/*.rb"]

        files.each do |file|
          puts "#{file}"
          Rubinius.compile_file file, true
        end

        exit 0
      end
    end

    def debugger
      @stage = "running the debugger"

      if Rubinius::Config['debug']
        require 'debugger'
        Debugger.start
      end
    end

    # Require any -r arguments
    def requires
      @stage = "requiring command line files"

      @requires.each { |file| require file }
    end

    # Evaluate any -e arguments
    def evals
      @stage = "evaluating command line code"

      eval(@evals.join("\n"), TOPLEVEL_BINDING, "-e", 1)
    rescue SystemExit => e
      @exit_code = e.status
    end

    # Run the script passed on the command line
    def script
      return unless @script and @evals.empty?

      @stage = "running #{@script}"
      Dir.chdir @directory if @directory

      if File.exist?(@script)
        $0 = @script

        CodeLoader.load_script @script, @debugging
      else
        if @script.suffix?(".rb")
          puts "Unable to find '#{@script}'"
          exit 1
        else
          prog = File.join @main_lib, "bin", "#{@script}.rb"
          if File.exist? prog
            $0 = prog
            load prog
          else
            raise LoadError, "Unable to find a script '#{@script}' to run"
          end
        end
      end
    rescue SystemExit => e
      @exit_code = e.status
    end

    # Run IRB unless we were passed -e, -S arguments or a script to run.
    def irb
      return if $0 or not @run_irb

      @stage = "running IRB"

      if Terminal
        repr = ENV['RBX_REPR'] || "bin/irb"
        $0 = repr
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
        $0 = "(eval)"
        CodeLoader.execute_script "p #{STDIN.read}"
      end
    end

    # Cleanup and at_exit processing.
    def epilogue
      @stage = "at_exit handler"

      until AtExit.empty?
        begin
          AtExit.shift.call
        rescue SystemExit => e
          @exit_code = e.status
        end
      end

      @stage = "object finalizers"
      GC.start
      ObjectSpace.run_finalizers

      # TODO: Fix these with better -X processing
      if Config['rbx.jit_stats']
        p VM.jit_info
      end

      if Config['rbx.gc_stats']
        Stats::GC.new.show
      end
    rescue Object => e
      e.render "An exception occurred #{@stage}"
      @exit_code = 1
    end

    # Exit.
    def done
      Process.exit! @exit_code
    end

    # Orchestrate everything.
    def main
      preamble
      system_load_path
      signals
      load_compiler
      preload
      options
      load_paths
      debugger
      requires
      evals
      script
      irb

    rescue SystemExit => e
      @exit_code = e.status

    rescue SyntaxError => e
      show_syntax_error(e)

      STDERR.puts "\nBacktrace:"
      STDERR.puts e.awesome_backtrace.show
      @exit_code = 1

    rescue Object => e
      e.render "An exception occurred #{@stage}"
      @exit_code = 1

    ensure
      epilogue
      done
    end

    # Creates an instance of the Loader and runs it. We catch any uncaught
    # exceptions here and report them before exiting.
    def self.main
      begin
        new.main
      rescue Object => exc
        puts "\n====================================="
        puts "Exception occurred during top-level exception output! (THIS IS BAD)"
        puts
        puts "Exception: #{exc.inspect} (#{exc.class})"
        @exit_code = 128
      end
    end
  end
end

Rubinius::Loader.main
