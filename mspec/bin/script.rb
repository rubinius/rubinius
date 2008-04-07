require 'mspec/runner/formatters/dotted'

# MSpecScript provides a skeleton for all the MSpec runner scripts.
# The front script, 'mspec', just overrides a couple methods in
# MSpecScript. It could subclass, but there is no real advantage
# to doing so. The rest of the scripts do subclass MSpecScript,
# mostly for uniformity. At this time, only mspec-tag needs special
# code in #initialize. If not for this, the approach used by 'mspec'
# would be just as simple.

class MSpecScript
  def self.config
    @config ||= { :path => ['.', 'spec'] }
  end

  def self.set(key, value)
    config[key] = value
  end

  def initialize
    config[:tags_dir]  = 'spec/tags'
    config[:formatter] = DottedFormatter
    config[:includes]  = []
    config[:excludes]  = []
    config[:patterns]  = []
    config[:xpatterns] = []
    config[:tags]      = []
    config[:xtags]     = []
    config[:atags]     = []
    config[:astrings]  = []
    config[:abort]     = true
  end

  def config
    MSpecScript.config
  end

  def load(name)
    return Kernel.load(name) if File.exist?(File.expand_path(name))

    config[:path].each do |dir|
      file = File.join dir, name
      return Kernel.load(file) if File.exist? file
    end
  end

  def register
    config[:formatter].new.register

    MatchFilter.new(:include, *config[:includes]).register   unless config[:includes].empty?
    MatchFilter.new(:exclude, *config[:excludes]).register   unless config[:excludes].empty?
    RegexpFilter.new(:include, *config[:patterns]).register  unless config[:patterns].empty?
    RegexpFilter.new(:exclude, *config[:xpatterns]).register unless config[:xpatterns].empty?
    TagFilter.new(:include, *config[:tags]).register         unless config[:tags].empty?
    TagFilter.new(:exclude, *config[:xtags]).register        unless config[:xtags].empty?

    DebugAction.new(config[:atags], config[:astrings]).register if config[:debugger]
    GdbAction.new(config[:atags], config[:astrings]).register   if config[:gdb]
  end

  def signals
    if config[:abort]
      Signal.trap "INT" do
        puts "\nSpec process aborted!"
        exit! 1
      end
    end
  end

  def self.main
    script = new
    script.load 'default.mspec'
    script.load '~/.mspecrc'
    script.options
    script.signals
    script.register
    script.run
  end
end
