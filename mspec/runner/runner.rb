require 'mspec/runner/formatters/dotted'
require 'mspec/runner/guards'

unless MSpec.engine?(:rubinius)
  # The useless use warnings are a crime against OO.
  def $stderr.write(msg)
    if msg =~ /useless use of/
      nil
    else
      super(msg)
    end
  end
end

class DescribeState
  def before_all
    @before_all ||= []
  end
  
  def before_each
    @before_each ||= []
  end

  def after_each
    @after_each ||= []
  end
  
  def after_all
    @after_all ||= []
  end
  
  def it
    @it ||= []
  end
end

class SpecRunner
  def initialize(formatter=nil)
    clear_filters
    @stack = []
    @formatter = formatter
    if @formatter.nil?
      @formatter = DottedFormatter.new
    end
    @formatter.start_timer
  end

  def formatter
    @formatter
  end
  
  def formatter=(formatter)
    @formatter = formatter
  end

  def escape(str)
    str.is_a?(Regexp) ? str : Regexp.new(Regexp.escape(str))
  end
  
  def convert_to_regexps(*args)
    args.inject([]) do |list, item|
      if item.is_a?(String) and File.exist?(item)
        if f = File.open(item, "r")
          f.each do |line|
            line.chomp!
            list << escape(line) unless line.empty?
          end
          f.close
        end
        list
      else
        list << escape(item)
      end
    end
  end
  
  def only(*args)
    @only.concat convert_to_regexps(*args)
  end
  
  def except(*args)
    @except.concat convert_to_regexps(*args)
  end
  
  def clear_filters
    @only = []
    @except = []
  end
  
  def skip?(example)
    @except.each { |re| return true if re.match(example) }
    return false if @only.empty?
    matched = @only.any? { |re| re.match(example) }
    return true unless matched
    return false
  end

  def before(at=:each, &block)
    case at
    when :each
      @stack.last.before_each << block
    when :all
      @stack.last.before_all << block
    else
    end
  end
  
  def after(at=:each, &block)
    case at
    when :each
      @stack.last.after_each << block
    when :all
      @stack.last.after_all << block
    end
  end
  
  def it(msg, &block)
    @stack.last.it << [msg, block]
  end
  
  def describe(*args, &block)
    @stack.push DescribeState.new
    dmsg = args.join " "
    formatter.before_describe(dmsg)

    begin
      block.call

      @stack.last.before_all.each { |ba| ba.call }
      @stack.last.it.each do |msg, b|
        unless skip?("#{dmsg} #{msg}")
          formatter.before_it(msg)
          begin
            begin
              @stack.last.before_each.each { |be| be.call }
              b.call
              Mock.verify_count
            rescue Exception => e
              formatter.exception(e)
            ensure
              @stack.last.after_each.each { |ae| ae.call }
              Mock.cleanup
            end
          rescue Exception => e
            formatter.exception(e)
          end
          formatter.after_it(msg)
        end
      end
    ensure
      @stack.last.after_all.each { |aa| aa.call }
      Mock.cleanup
    end

    formatter.after_describe(dmsg)
    @stack.pop
  end
end

module MSpec
  def self.describe(mod, msg, &block)
    stack.push RunState.new
    
    current.describe(mod, msg, &block)
    current.process
  end
  
  def self.actions(action)
    retrieve(action).each { |obj| obj.send action }
  end
  
  def self.register_files(files)
    store :files, files
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
  #   :start    before any specs are run
  #   :load     before a spec file is loaded
  #   :before   before a single spec is run
  #   :after    after a single spec is run
  #   :unload   after a spec file is run
  #   :finish   after all specs are run
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
    value << action
  end
  
  def self.unregister(symbol, action)
    if value = retrieve(symbol)
      value.delete action
    end
  end
end
