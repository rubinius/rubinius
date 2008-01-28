require 'mspec/mspec'

class RunState
  def initialize
    @start = []
    @before = []
    @after = []
    @finish = []
    @spec = []
  end
  
  def state
    @state
  end
  
  def before(at=:each, &block)
    case at
    when :each
      @before << block
    when :all
      @start << block
    end
  end
  
  def after(at=:each, &block)
    case at
    when :each
      @after << block
    when :all
      @finish << block
    end
  end
  
  def it(desc, &block)
    @spec << [desc, block]
  end
  
  def describe(mod, desc=nil, &block)
    @describe = desc ? "#{mod} #{desc}" : mod.to_s
    @block = block
  end
  
  def protect(what, blocks)
    Array(blocks).each { |block| MSpec.protect what, &block }
  end
  
  def process
    protect @describe, @block
    protect "before :all", @start
    @spec.each do |desc, spec|
      @state = SpecState.new @describe, desc
      protect "before :each", @before
      protect nil, spec
      protect "after :each", @after
      protect "Mock.cleanup", lambda { Mock.cleanup }
      @state = nil
    end
    protect "after :all", @finish
  end
end

class SpecState
  def initialize(describe, it)
    @describe = describe
    @it = it
  end
  
  def describe
    @describe
  end
  
  def it
    @it
  end
  
  def description
    "#{@describe} #{@it}"
  end
  
  def exceptions
    @exceptions ||= []
  end
  
  def exception?
    not exceptions.empty?
  end
end
