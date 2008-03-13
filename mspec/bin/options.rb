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

class SpecConfig
  attr_accessor :formatter, :includes, :excludes,
                :patterns, :xpatterns, :tags, :xtags,
                :tagger, :outcome, :tag, :comment, :atags, :astrings,
                :debugger, :gdb, :full_abort

  def initialize(command, options)
    @options = options
    @formatter = DottedFormatter
    @includes = []
    @excludes = []
    @patterns = []
    @xpatterns = []
    @tags = []
    @xtags = []
    @atags = []
    @astrings = []
    @full_abort = true
  end

  def register
    if (@debugger or @gdb) and @atags.empty? and @astrings.empty?
      puts "Missing --action-tag or --action-string."
      puts @options
      exit 1
    end

    @formatter.new.register

    MatchFilter.new(:include, *@includes).register unless @includes.empty?
    MatchFilter.new(:exclude, *@excludes).register unless @excludes.empty?
    RegexpFilter.new(:include, *@patterns).register unless @patterns.empty?
    RegexpFilter.new(:exclude, *@xpatterns).register unless @xpatterns.empty?
    TagFilter.new(:include, *@tags).register unless @tags.empty?
    TagFilter.new(:exclude, *@xtags).register unless @xtags.empty?

    DebugAction.new(@atags, @astrings).register if @debugger
    GdbAction.new(@atags, @astrings).register if @gdb

    if @tagger
      tag = SpecTag.new(@tag)
      tagger = TagAction.new(@tagger, @outcome, tag.tag, tag.comment, @atags, @astrings)
      tagger.register
    end

    if @full_abort
      Signal.trap "INT" do
        puts "\nSpec process aborted!"
        exit! 1
      end
    end
  end
end

class SpecOptions
  attr_reader :options, :config

  def initialize(command, *args)
    @options = OptionParser.new(*args) do |opts|
      opts.banner = "mspec #{command} [options] (FILE|DIRECTORY|GLOB)+"
      opts.separator ""
    end

    @config = SpecConfig.new command, @options
  end

  def add_formatters
    @options.on("-f", "--format FORMAT", String,
                "Formatter for reporting: s:specdoc|d:dotted|h:html|u:unitdiff|a:*:spin") do |o|
      case o
      when 's', 'specdoc'
        @config.formatter = SpecdocFormatter
      when 'h', 'html'
        @config.formatter = HtmlFormatter
      when 'd', 'dot', 'dotted'
        @config.formatter = DottedFormatter
      when 'u', 'unit', 'unitdiff'
        @config.formatter = UnitdiffFormatter
      when 'm', 'summary'
        @config.formatter = SummaryFormatter
      when 'a', '*', 'spin'
        @config.formatter = SpinnerFormatter
      else
        puts "Unknown format: #{o}"
        puts @options
        exit
      end
    end
  end

  def add_filters
    @options.on("-e", "--example STR", String,
            "Run examples with descriptions matching STR") do |o|
      @config.includes << o
    end
    @options.on("-E", "--exclude STR", String,
            "Exclude examples with descriptions matching STR") do |o|
      @config.excludes << o
    end
    @options.on("-p", "--pattern PATTERN", Regexp,
            "Run examples with descriptions matching PATTERN") do |o|
      @config.patterns << o
    end
    @options.on("-P", "--excl-pattern PATTERN", Regexp,
            "Exclude examples with descriptions matching PATTERN") do |o|
      @config.xpatterns << o
    end
    @options.on("-g", "--tag TAG", String,
            "Run examples with descriptions matching ones tagged with TAG") do |o|
      @config.tags << o
    end
    @options.on("-G", "--excl-tag TAG", String,
            "Exclude examples with descriptions matching ones tagged with TAG") do |o|
      @config.xtags << o
    end
  end

  def add_pretend
    @options.on("-Z", "--dry-run",
                "Invoke formatters and other actions, but don't execute the specs") do
      MSpec.register_mode :pretend
    end
  end

  def add_verbose
    @options.on("-V", "--verbose", "Output the name of each file processed") do
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

    @options.on("-m", "--marker MARKER", String,
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
    @options.on("--int-spec", "Control-C interupts the current spec only") do
      @config.full_abort = false
    end
  end

  def add_verify
    @options.on("-Y", "--verify",
               "Verify that guarded specs pass and fail as expected") { MSpec.set_mode :verify }
    @options.on("-O", "--report", "Report guarded specs") { MSpec.set_mode :report }
  end

  def add_tagging
    @options.on("-N", "--add TAG", String,
                "Add TAG with format 'tag' or 'tag(comment)' (see -Q, -F, -L)") do |o|
      @config.tagger = :add
      @config.tag = "#{o}:"
    end
    @options.on("-R", "--del TAG", String,
                "Delete TAG (see -Q, -F, -L)") do |o|
      @config.tagger = :del
      @config.tag = "#{o}:"
      @config.outcome = :pass
    end
    @options.on("-Q", "--pass", "Apply action to specs that pass (default for --del)") do
      @config.outcome = :pass
    end
    @options.on("-F", "--fail", "Apply action to specs that fail (default for --add)") do
      @config.outcome = :fail
    end
    @options.on("-L", "--all", "Apply action to all specs") do
      @config.outcome = :all
    end
  end

  def add_action_filters
    @options.on("-K", "--action-tag TAG", String,
                "Spec descriptions marked with TAG will trigger the specified action") do |o|
      @config.atags << o
    end
    @options.on("-S", "--action-string STR", String,
                "Spec descriptions matching STR will trigger the specified action") do |o|
      @config.astrings << o
    end
  end

  def add_actions
    @options.on("--spec-debug",
                "Invoke the debugger when a spec description matches (see -K, -S)") do
      @config.debugger = true
    end
    @options.on("--spec-gdb",
                "Invoke Gdb when a spec description matches (see -K, -S)") do
      @config.gdb = true
    end
  end

  def add_help
    @options.on("-v", "--version", "Show version") do
      puts "MSpec #{MSpec::VERSION}"
      exit
    end
    @options.on("-h", "--help", "Show this message") do
      puts @options
      exit
    end
  end

  def parse
    @options.parse ENV['MSPEC_OPTIONS'].split("\n")
  end
end
