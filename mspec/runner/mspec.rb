require 'mspec/runner/state'
require 'mspec/runner/tag'
require 'fileutils'

module MSpec

  @before  = nil
  @enter   = nil
  @exclude = nil
  @include = nil
  @leave   = nil
  @mode    = nil
  @unload  = nil

  def self.describe(mod, msg, &block)
    stack.push RunState.new
    
    current.describe(mod, msg, &block)
    current.process
    
    stack.pop
  end
  
  def self.process
    actions :start
    files
    actions :finish
  end
  
  def self.files
    return unless files = retrieve(:files)

    files.each do |file|
      store :file, file
      actions :load
      protect("loading #{file}") { Kernel.load file }
      actions :unload
    end
  end
  
  def self.actions(action, *args)
    actions = retrieve(action)
    actions.each { |obj| obj.send action, *args } if actions
  end
  
  def self.register_exit(code)
    store :exit, code
  end
  
  def self.exit_code
    retrieve(:exit).to_i
  end
  
  def self.register_files(files)
    store :files, files
  end
  
  def self.register_tags_path(path)
    store :tags_path, path
  end
  
  def self.register_mode(mode)
    store :mode, mode
  end
  
  def self.retrieve(symbol)
    instance_variable_get :"@#{symbol}"
  end
  
  def self.store(symbol, value)
    instance_variable_set :"@#{symbol}", value
  end
  
  # This method is used for registering actions that are
  # run at particular points in the spec cycle:
  #   :start        before any specs are run
  #   :load         before a spec file is loaded
  #   :enter        before a describe block is run
  #   :before       before a single spec is run
  #   :expectation  before a 'should', 'should_receive', etc.
  #   :after        after a single spec is run
  #   :leave        after a describe block is run
  #   :unload       after a spec file is run
  #   :finish       after all specs are run
  #
  # Objects registered as actions above should respond to
  # a method of the same name. For example, if an object
  # is registered as a :start action, it should respond to
  # a #start method call.
  #
  # Additionally, there are two "action" lists for 
  # filtering specs:
  #   :include  return true if the spec should be run
  #   :exclude  return true if the spec should NOT be run
  #
  def self.register(symbol, action)
    unless value = retrieve(symbol)
      value = store symbol, []
    end
    value << action unless value.include? action
  end
  
  def self.unregister(symbol, action)
    if value = retrieve(symbol)
      value.delete action
    end
  end
  
  def self.protect(msg, &block)
    begin
      block.call
    rescue Exception => e
      register_exit 1
      if current and current.state
        current.state.exceptions << [msg, e]
      else
        STDERR.write "\nAn exception occurred in #{msg}:\n#{e.class}: #{e.message.pretty_inspect}\n"
        STDERR.write "#{e.backtrace.join "\n"}"
      end
    end
  end
  
  def self.stack
    @stack ||= []
  end
  
  def self.current
    stack.last
  end
  
  def self.verify_mode?
    @mode == :verify
  end
  
  def self.report_mode?
    @mode == :report
  end
  
  def self.pretend_mode?
    @mode == :pretend
  end

  def self.tags_path
    retrieve(:tags_path) || ".tags"
  end
  
  def self.tags_file
    path = tags_path
    file = retrieve :file
    tags_file = File.basename(file, '.*').sub(/_spec$/, '_tags') + '.txt'
    
    if path[0] == ?/
      m = file.match %r[.*spec/(.*)/.*_spec.rb]
      tags_path = m ? File.join(path, m[1]) : path
    else
      tags_path = File.join(File.dirname(file), path)
    end
    File.join tags_path, tags_file
  end
  
  def self.read_tags(*keys)
    tags = []
    file = tags_file
    if File.exist? file
      File.open(file, "r") do |f|
        f.each_line do |line|
          tag = SpecTag.new line.chomp
          tags << tag if keys.include? tag.tag
        end
      end
    end
    tags
  end
  
  def self.write_tag(tag)
    string = tag.to_s
    file = tags_file
    path = File.dirname file
    FileUtils.mkdir_p(path) unless File.exist?(path)
    if File.exist? file
      File.open(file, "r") do |f|
        f.each_line { |line| return if line.chomp == string }
      end
    end
    File.open(file, "a") { |f| f.puts string }
  end
  
  def self.delete_tag(tag)
    pattern = /#{tag.tag}.*#{tag.description}/
    file = tags_file
    if File.exist? file
      lines = IO.readlines(file)
      File.open(file, "w") do |f|
        lines.each { |line| f.puts line unless pattern =~ line.chomp }
      end
    end
  end
end
