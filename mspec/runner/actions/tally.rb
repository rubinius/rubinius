class Tally
  attr_reader :files, :examples, :expectations, :failures, :errors

  def initialize
    @files = @examples = @expectations = @failures = @errors = 0
  end

  def files!(add=1)
    @files += add
  end

  def examples!(add=1)
    @examples += add
  end

  def expectations!(add=1)
    @expectations += add
  end

  def failures!(add=1)
    @failures += add
  end

  def errors!(add=1)
    @errors += add
  end

  def format
    [ [@files, 'file'],
      [@examples, 'example'],
      [@expectations, 'expectation'],
      [@failures, 'failure'],
      [@errors, 'error']
    ].map { |count, word| pluralize count, word }.join(", ")
  end

  def pluralize(count, singular)
    "#{count} #{singular}#{'s' unless count == 1}"
  end
  private :pluralize
end

class TallyAction
  attr_reader :counter

  def initialize
    @counter = Tally.new
  end

  def register
    MSpec.register :load, self
    MSpec.register :after, self
    MSpec.register :expectation, self
  end

  def unregister
    MSpec.unregister :load, self
    MSpec.unregister :after, self
    MSpec.unregister :expectation, self
  end

  def load
    @counter.files!
  end

  def expectation(state)
    @counter.expectations!
  end

  def after(state)
    @counter.examples!
    state.exceptions.each do |msg, exc|
      state.failure?(exc) ? @counter.failures! : @counter.errors!
    end
  end

  def format
    @counter.format
  end
end
