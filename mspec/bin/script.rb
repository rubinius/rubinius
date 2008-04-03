# MSpecScript provides a skeleton for all the MSpec runner scripts.
# The front script, 'mspec', just overrides a couple methods in
# MSpecScript. It could subclass, but there is no real advantage
# to doing so. The rest of the scripts do subclass MSpecScript,
# mostly for uniformity. At this time, only mspec-tag needs special
# code in #initialize. If not for this, the approach used by 'mspec'
# would be just as simple.

class MSpecScript
  Config = {}
  Config[:path] = ['.', 'spec']

  def initialize
    Config[:tags_dir]  = 'spec/tags'
    Config[:formatter] = DottedFormatter
    Config[:includes]  = []
    Config[:excludes]  = []
    Config[:patterns]  = []
    Config[:xpatterns] = []
    Config[:tags]      = []
    Config[:xtags]     = []
    Config[:atags]     = []
    Config[:astrings]  = []
    Config[:abort]     = true
  end

  def config(name)
    Config[:path].each do |dir|
      file = File.join dir, name
      return load(file) if File.exist? file
    end
  end

  def register
    Config[:formatter].new.register

    MatchFilter.new(:include, *Config[:includes]).register   unless Config[:includes].empty?
    MatchFilter.new(:exclude, *Config[:excludes]).register   unless Config[:excludes].empty?
    RegexpFilter.new(:include, *Config[:patterns]).register  unless Config[:patterns].empty?
    RegexpFilter.new(:exclude, *Config[:xpatterns]).register unless Config[:xpatterns].empty?
    TagFilter.new(:include, *Config[:tags]).register         unless Config[:tags].empty?
    TagFilter.new(:exclude, *Config[:xtags]).register        unless Config[:xtags].empty?

    DebugAction.new(Config[:atags], Config[:astrings]).register if Config[:debugger]
    GdbAction.new(Config[:atags], Config[:astrings]).register   if Config[:gdb]
  end

  def signals
    if Config[:abort]
      Signal.trap "INT" do
        puts "\nSpec process aborted!"
        exit! 1
      end
    end
  end

  def self.main
    script = new
    script.config 'default.mspec'
    script.options
    script.signals
    script.register
    script.run
  end
end
