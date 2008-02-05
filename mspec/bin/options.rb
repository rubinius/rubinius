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
                :patterns, :xpatterns, :tags, :xtags
  
  def initialize
    @formatter = DottedFormatter
    @includes = []
    @excludes = []
    @patterns = []
    @xpatterns = []
    @tags = []
    @xtags = []
  end
  
  def register
    @formatter.new.register
  end
end

class SpecOptions
  attr_reader :options, :config
  
  def initialize(command, *args)
    @config = SpecConfig.new
    
    @options = OptionParser.new(*args) do |opts|
      opts.banner = "mspec #{command} [options] (FILE|DIRECTORY|GLOB)+"
      opts.separator ""

      opts.on("-f", "--format FORMAT", String, 
              "Formatter for reporting: s:specdoc|d:dotted|h:html|u:unitdiff") do |o|
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
        else
          puts "Unknown format: #{o}"
          puts opts
          exit
        end
      end
      opts.on("-e", "--example STRING", String,
              "Execute examples with descriptions matching STRING") do |o|
        puts "I'm here! #{o}"
        @config.includes << o
      end
      opts.on("-E", "--exclude STRING", String,
              "Exclude examples with descriptions matching STRING") do |o|
        @config.excludes << o
      end
      opts.on("-p", "--pattern PATTERN", Regexp,
              "Execute examples with descriptions matching PATTERN") do |o|
        @config.patterns << o
      end
      opts.on("-P", "--exclude-pattern PATTERN", Regexp,
              "Exclude examples with descriptions matching PATTERN") do |o|
        @config.xpatterns << o
      end
      opts.on("-g", "--tag TAG", String,
              "Execute examples with descriptions matching ones tagged with TAG") do |o|
        @config.tags << o
      end
      opts.on("-G", "--exclude-tag TAG", String,
              "Exclude examples with descriptions matching ones tagged with TAG") do |o|
        @config.xtags << o
      end
      opts.on("-V", "--verbose", "Output the name of each file processed") do
        obj = Object.new
        def obj.start
          @width = MSpec.retrieve(:files).inject(0) { |max, f| f.size > max ? f.size : max }
        end
        def obj.load
          print "\n#{file.ljust(@width)}"
        end
        MSpec.register :start, obj
        MSpec.register :load, obj
      end
      opts.on("-m", "--marker MARKER", String,
              "Outout MARKER for each file processed") do |o|
        obj = Object.new
        def obj.load
          print o
        end
        MSpec.register :load, obj
      end
      opts.on("-v", "--version", "Show version") do
        puts "MSpec #{MSpec::VERSION}"
        exit
      end
      opts.on("-h", "--help", "Show this message") do
        puts opts
        exit
      end
    end
  end
  
  def parse
    @options.parse ENV['MSPEC_OPTIONS'].split
  end
end
