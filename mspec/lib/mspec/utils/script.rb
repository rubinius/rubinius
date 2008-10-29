require 'mspec/runner/formatters/dotted'

# MSpecScript provides a skeleton for all the MSpec runner scripts.

class MSpecScript
  def self.config
    @config ||= {
      :path => ['.', 'spec'],
      :config_ext => '.mspec'
    }
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
    config[:profiles]  = []
    config[:xprofiles] = []
    config[:atags]     = []
    config[:astrings]  = []
    config[:ltags]     = []
    config[:abort]     = true
  end

  def config
    MSpecScript.config
  end

  def load(name)
    names = [name]
    unless name[-6..-1] == config[:config_ext]
      names << name + config[:config_ext]
    end

    names.each do |name|
      return Kernel.load(name) if File.exist?(File.expand_path(name))

      config[:path].each do |dir|
        file = File.join dir, name
        return Kernel.load(file) if File.exist? file
      end
    end
  end

  def register
    config[:formatter].new(config[:output]).register if config[:formatter]

    MatchFilter.new(:include, *config[:includes]).register    unless config[:includes].empty?
    MatchFilter.new(:exclude, *config[:excludes]).register    unless config[:excludes].empty?
    RegexpFilter.new(:include, *config[:patterns]).register   unless config[:patterns].empty?
    RegexpFilter.new(:exclude, *config[:xpatterns]).register  unless config[:xpatterns].empty?
    TagFilter.new(:include, *config[:tags]).register          unless config[:tags].empty?
    TagFilter.new(:exclude, *config[:xtags]).register         unless config[:xtags].empty?
    ProfileFilter.new(:include, *config[:profiles]).register  unless config[:profiles].empty?
    ProfileFilter.new(:exclude, *config[:xprofiles]).register unless config[:xprofiles].empty?

    DebugAction.new(config[:atags], config[:astrings]).register if config[:debugger]
    GdbAction.new(config[:atags], config[:astrings]).register   if config[:gdb]
  end

  def signals
    if config[:abort]
      Signal.trap "INT" do
        puts "\nProcess aborted!"
        exit! 1
      end
    end
  end

  def files(list)
    list.inject([]) do |files, item|
      stat = File.stat(File.expand_path(item))
      files << item if stat.file?
      files.concat(Dir[item+"/**/*_spec.rb"].sort) if stat.directory?
      files
    end
  end

  def self.main
    $VERBOSE = nil unless ENV['OUTPUT_WARNINGS']
    script = new
    script.load 'default.mspec'
    script.load '~/.mspecrc'
    script.options
    script.signals
    script.register
    script.run
  end
end
