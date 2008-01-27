require 'mspec/mspec'

class RunState
  def initialize
    @start = []
    @before = []
    @after = []
    @finish = []
    @spec = []
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
  
  def it(msg, &block)
    @spec << [msg, block]
  end
  
  def describe(mod, msg=nil, &block)
    @describe = msg ? "#{mod} #{msg}" : mod.to_s
    @block = block
  end
  
  def protect(msg, blocks)
    Array(blocks).each { |block| MSpec.protect msg, &block }
  end
  
  def process
    protect "before :all", @start
    @spec.each do |msg, spec|
      protect "before :each", @before
      protect "", spec
      protect "after :each", @after
      protect "Mock.cleanup", lambda { Mock.cleanup }
    end
    protect "after :all", @finish
  end
end
