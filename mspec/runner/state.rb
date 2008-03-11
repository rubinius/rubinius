require 'mspec/runner/mspec'

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
    state = SpecState.new @describe, desc
    @spec << [desc, block, state] unless state.filtered?
  end
  
  def describe(mod, desc=nil, &block)
    @describe = desc ? "#{mod} #{desc}" : mod.to_s
    @block = block
  end
  
  def protect(what, blocks, check=true)
    return if check and MSpec.pretend_mode?
    Array(blocks).each { |block| MSpec.protect what, &block }
  end
  
  def process
    protect @describe, @block, false
    return unless @spec.any? { |desc, spec, state| state.unfiltered? }
    
    MSpec.actions :enter, @describe
    protect "before :all", @start
    @spec.each do |desc, spec, state|
      @state = state
      MSpec.actions :before, state
      protect "before :each", @before
      protect nil, spec
      protect "after :each", @after
      protect "Mock.verify_count", lambda { Mock.verify_count }
      protect "Mock.cleanup", lambda { Mock.cleanup }
      MSpec.actions :after, state
      @state = nil
    end
    protect "after :all", @finish
    MSpec.actions :leave
  end
end

class SpecState
  def initialize(describe, it)
    @describe = describe
    @it = it
    @unfiltered = nil
  end
  
  def describe
    @describe
  end
  
  def it
    @it
  end
  
  def description
    @description ||= "#{@describe} #{@it}"
  end
  
  def exceptions
    @exceptions ||= []
  end
  
  def exception?
    not exceptions.empty?
  end
  
  def unfiltered?
    unless @unfiltered
      incl = MSpec.retrieve(:include) || []
      excl = MSpec.retrieve(:exclude) || []
      @unfiltered   = incl.empty? || incl.any? { |f| f === description }
      @unfiltered &&= excl.empty? || !excl.any? { |f| f === description }
    end
    @unfiltered
  end
  
  def filtered?
    not unfiltered?
  end
  
  def failure?(exception)
    exception.is_a?(ExpectationNotMetError)
  end
end
