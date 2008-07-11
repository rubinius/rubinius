require 'mspec/runner/mspec'
require 'mspec/runner/example'

# Holds the state of the +describe+ block that is being
# evaluated. Every example (i.e. +it+ block) is evaluated
# in a context, which may include state set up in <tt>before
# :each</tt> or <tt>before :all</tt> blocks.
#
#--
# A note on naming: this is named _ContextState_ rather
# than _DescribeState_ because +describe+ is the keyword
# in the DSL for refering to the context in which an example
# is evaluated, just as +it+ refers to the example itself.
#++
class ContextState
  attr_reader :state

  def initialize
    @start  = []
    @before = []
    @after  = []
    @finish = []
    @spec   = []
    @mock_verify         = lambda { Mock.verify_count }
    @mock_cleanup        = lambda { Mock.cleanup }
    @expectation_missing = lambda { raise ExpectationNotFoundError }
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
    state = ExampleState.new @describe, desc
    @spec << [desc, block, state] unless state.filtered?
  end

  def describe(mod, desc=nil, &block)
    @describe = desc ? "#{mod} #{desc}" : mod.to_s
    @block = block
  end

  def protect(what, blocks, check=true)
    return true if check and MSpec.pretend_mode?
    Array(blocks).all? { |block| MSpec.protect what, &block }
  end

  def process
    protect @describe, @block, false
    return unless @spec.any? { |desc, spec, state| state.unfiltered? }

    MSpec.shuffle @spec if MSpec.randomize?
    MSpec.actions :enter, @describe

    if protect "before :all", @start
      @spec.each do |desc, spec, state|
        @state = state
        MSpec.actions :before, state

        if protect("before :each", @before)
          MSpec.clear_expectations
          protect nil, spec
          if spec
            MSpec.actions :example, state, spec
            protect nil, @expectation_missing unless MSpec.expectation?
          end
          protect "after :each", @after
          protect "Mock.verify_count", @mock_verify
        end

        protect "Mock.cleanup", @mock_cleanup
        MSpec.actions :after, state
        @state = nil
      end
      protect "after :all", @finish
    else
      protect "Mock.cleanup", @mock_cleanup
    end

    MSpec.actions :leave
  end
end
