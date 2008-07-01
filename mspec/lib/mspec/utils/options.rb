# FIXME: There are essentially two options for these runner
# scripts: 1. all in one file, 2. in separate files. The fact
# that the subcommands share a number of options weighs
# toward having a single script. However, the subcommands
# also take specialized options which can be confusing. Added
# to this the support for switches to specify a different
# implementation and the balance swings to separate scripts.
#
# Now, the fun begins. The main script needs a way to specify
# certain command line options and then pass the rest to the
# invoked subscript. Unfortunately, this possibility does not
# exist in the universe inhabited by OptionParser or GetoptLong.
#
# The #filter! method below does not properly handle optional
# arguments to a switch.
class OptionParser
  class Switch
    def consume?(opt)
      if opt == short.to_s or opt == long.to_s
        return arg ? 2 : 1
      elsif opt[0..1] == short.to_s and opt.size > 2
        return 1
      else
        return 0
      end
    end
  end

  def filter!(argv)
    @stack.inject(options=[]) do |list, entry|
      entry.list.each do |switch|
        next unless switch.kind_of? OptionParser::Switch
        list << switch
      end
      list
    end

    filtered = []
    recognized = []
    consume = nil
    until argv.empty?
      opt = argv.shift
      options.find { |o| consume = o.consume?(opt); consume != 0 }
      if consume == 0
        filtered << opt
      else
        recognized << opt
        recognized << argv.shift if consume == 2
      end
    end
    argv.replace recognized
    filtered
  end
end

require 'mspec/version'

# MSpecOptions wraps OptionParser and provides a composable set of
# options that the runner scripts pick from.
class MSpecOptions
  attr_reader :parser

  def initialize(config, command, *args)
    @parser = OptionParser.new(*args) do |opts|
      opts.banner = "mspec #{command} [options] (FILE|DIRECTORY|GLOB)+"
      opts.separator ""
    end

    @config = config
  end

  def add_config(&block)
    on("-B", "--config FILE", String,
       "Load FILE containing configuration options", &block)
  end

  def add_name
    on("-n", "--name RUBY_NAME", String,
       "Set the value of RUBY_NAME (used to determine the implementation)") do |n|
      Object.const_set :RUBY_NAME, n
    end
  end

  def add_targets
    on("-t", "--target TARGET", String,
       "Implementation to run the specs, where:") do |t|
      case t
      when 'r', 'ruby'
        @config[:target] = 'ruby'
        @config[:flags] << '-v'
      when 'r19', 'ruby19'
        @config[:target] = 'ruby19'
      when 'x', 'rubinius'
        @config[:target] = 'shotgun/rubinius'
      when 'X', 'rbx'
        @config[:target] = 'rbx'
      when 'j', 'jruby'
        @config[:target] = 'jruby'
      else
        @config[:target] = t
      end
    end

    separator ""
    separator "   'r' or 'ruby'     invokes ruby in PATH"
    separator "   'r19' or 'ruby19' invokes ruby19 in PATH"
    separator "   'x' or 'rubinius' invokes ./shotgun/rubinius"
    separator "   'X' or 'rbx'      invokes rbx in PATH"
    separator "   'j' or 'jruby'    invokes jruby in PATH\n"

    on("-T", "--target-opt OPT", String,
       "Pass OPT as a flag to the target implementation") do |t|
      @config[:flags] << t
    end
    on("-I", "--include DIR", String,
       "Pass DIR through as the -I option to the target") do |d|
      @config[:includes] << "-I#{d}"
    end
    on("-r", "--require LIBRARY", String,
       "Pass LIBRARY through as the -r option to the target") do |f|
      @config[:requires] << "-r#{f}"
    end
  end

  def add_formatters
    on("-f", "--format FORMAT", String,
       "Formatter for reporting, where FORMAT is one of:") do |o|
      case o
      when 's', 'spec', 'specdoc'
        @config[:formatter] = SpecdocFormatter
      when 'h', 'html'
        @config[:formatter] = HtmlFormatter
      when 'd', 'dot', 'dotted'
        @config[:formatter] = DottedFormatter
      when 'u', 'unit', 'unitdiff'
        @config[:formatter] = UnitdiffFormatter
      when 'm', 'summary'
        @config[:formatter] = SummaryFormatter
      when 'a', '*', 'spin'
        @config[:formatter] = SpinnerFormatter
      when 'y', 'yaml'
        @config[:formatter] = YamlFormatter
      else
        puts "Unknown format: #{o}"
        puts @parser
        exit
      end
    end
    separator("")
    separator("       s, spec, specdoc         SpecdocFormatter")
    separator("       h, html,                 HtmlFormatter")
    separator("       d, dot, dotted           DottedFormatter")
    separator("       u, unit, unitdiff        UnitdiffFormatter")
    separator("       m, summary               SummaryFormatter")
    separator("       a, *, spin               SpinnerFormatter")
    separator("       y, yaml                  YamlFormatter\n")
    on("-o", "--output FILE", String,
       "Write formatter output to FILE") do |f|
      @config[:output] = f
    end
  end

  def add_filters
    on("-e", "--example STR", String,
       "Run examples with descriptions matching STR") do |o|
      @config[:includes] << o
    end
    on("-E", "--exclude STR", String,
       "Exclude examples with descriptions matching STR") do |o|
      @config[:excludes] << o
    end
    on("-p", "--pattern PATTERN", Regexp,
       "Run examples with descriptions matching PATTERN") do |o|
      @config[:patterns] << o
    end
    on("-P", "--excl-pattern PATTERN", Regexp,
       "Exclude examples with descriptions matching PATTERN") do |o|
      @config[:xpatterns] << o
    end
    on("-g", "--tag TAG", String,
       "Run examples with descriptions matching ones tagged with TAG") do |o|
      @config[:tags] << o
    end
    on("-G", "--excl-tag TAG", String,
       "Exclude examples with descriptions matching ones tagged with TAG") do |o|
      @config[:xtags] << o
    end
    on("-w", "--profile FILE", String,
       "Run examples for methods listed in the profile FILE") do |f|
      @config[:profiles] << f
    end
    on("-W", "--excl-profile FILE", String,
       "Exclude examples for methods listed in the profile FILE") do |f|
      @config[:xprofiles] << f
    end
  end

  def add_pretend
    on("-Z", "--dry-run",
       "Invoke formatters and other actions, but don't execute the specs") do
      MSpec.register_mode :pretend
    end
  end

  def add_randomize
    on("-H", "--random",
       "Randomize the list of spec files") do
      MSpec.randomize
    end
  end

  def add_verbose
    on("-V", "--verbose", "Output the name of each file processed") do
      obj = Object.new
      def obj.start
        @width = MSpec.retrieve(:files).inject(0) { |max, f| f.size > max ? f.size : max }
      end
      def obj.load
        file = MSpec.retrieve :file
        print "\n#{file.ljust(@width)}"
      end
      MSpec.register :start, obj
      MSpec.register :load, obj
    end

    on("-m", "--marker MARKER", String,
       "Output MARKER for each file processed") do |o|
      obj = Object.new
      obj.instance_variable_set :@marker, o
      def obj.load
        print @marker
      end
      MSpec.register :load, obj
    end
  end

  def add_interrupt
    on("--int-spec", "Control-C interupts the current spec only") do
      @config[:abort] = false
    end
  end

  def add_verify
    on("-Y", "--verify",
       "Verify that guarded specs pass and fail as expected") do
      MSpec.set_mode :verify
    end
    on("-O", "--report", "Report guarded specs") do
      MSpec.set_mode :report
    end
  end

  def add_tagging
    on("-N", "--add TAG", String,
       "Add TAG with format 'tag' or 'tag(comment)' (see -Q, -F, -L)") do |o|
      @config[:tagger] = :add
      @config[:tag] = "#{o}:"
    end
    on("-R", "--del TAG", String,
       "Delete TAG (see -Q, -F, -L)") do |o|
      @config[:tagger] = :del
      @config[:tag] = "#{o}:"
      @config[:outcome] = :pass
    end
    on("-Q", "--pass", "Apply action to specs that pass (default for --del)") do
      @config[:outcome] = :pass
    end
    on("-F", "--fail", "Apply action to specs that fail (default for --add)") do
      @config[:outcome] = :fail
    end
    on("-L", "--all", "Apply action to all specs") do
      @config[:outcome] = :all
    end
  end

  def add_action_filters
    on("-K", "--action-tag TAG", String,
       "Spec descriptions marked with TAG will trigger the specified action") do |o|
      @config[:atags] << o
    end
    on("-S", "--action-string STR", String,
       "Spec descriptions matching STR will trigger the specified action") do |o|
      @config[:astrings] << o
    end
  end

  def add_actions
    on("--spec-debug",
       "Invoke the debugger when a spec description matches (see -K, -S)") do
      @config[:debugger] = true
    end
    on("--spec-gdb",
       "Invoke Gdb when a spec description matches (see -K, -S)") do
      @config[:gdb] = true
    end
  end

  def add_version
    on("-v", "--version", "Show version") do
      puts "#{File.basename $0} #{MSpec::VERSION}"
      exit
    end
  end

  def add_help
    on("-h", "--help", "Show this message") do
      puts @parser
      exit
    end
  end

  def on(*args, &block)
    @parser.on(*args, &block)
  end

  def separator(str)
    @parser.separator str
  end

  def parse(argv=ARGV)
    result = @parser.parse argv

    if (@config[:debugger] || @config[:gdb]) &&
        @config[:atags].empty? && @config[:astrings].empty?
      puts "Missing --action-tag or --action-string."
      puts @parser
      exit 1
    end

    result
  rescue OptionParser::ParseError => e
    puts @parser
    puts
    puts e
    exit 1
  end
end
