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
    MSpec.register :load,        self
    MSpec.register :exception,   self
    MSpec.register :example,     self
    MSpec.register :expectation, self
  end

  def unregister
    MSpec.unregister :load,        self
    MSpec.unregister :exception,   self
    MSpec.unregister :example,     self
    MSpec.unregister :expectation, self
  end

  def load
    @counter.files!
  end

  # Callback for the MSpec :expectation event. Increments the
  # tally of expectations (e.g. #should, #should_receive, etc.).
  def expectation(state)
    @counter.expectations!
  end

  # Callback for the MSpec :exception event. Increments the
  # tally of errors and failures.
  def exception(exception)
    exception.failure? ? @counter.failures! : @counter.errors!
  end

  # Callback for the MSpec :example event. Increments the tally
  # of examples.
  def example(state, block)
    @counter.examples!
  end

  def format
    @counter.format
  end
end
