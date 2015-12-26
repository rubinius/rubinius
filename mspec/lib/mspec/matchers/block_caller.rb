class BlockingMatcher
  def initialize(timeout = 0.1)
    @timeout = timeout
  end

  def matches?(block)
    blocking = true

    thread = Thread.new do
      block.call

      blocking = false
    end

    thread.join(@timeout)
    thread.kill

    blocking
  end

  def failure_message
    ['Expected the given Proc', 'to block the caller']
  end

  def negative_failure_message
    ['Expected the given Proc', 'to not block the caller']
  end
end

class Object
  def block_caller(timeout = 0.1)
    BlockingMatcher.new(timeout)
  end
end
