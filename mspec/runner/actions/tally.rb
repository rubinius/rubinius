class TallyAction
  attr_reader :files, :examples, :expectations, :failures, :errors
  
  def initialize
    @files = @examples = @expectations = @failures = @errors = 0
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
    @files += 1
  end
  
  def expectation(state)
    @expectations += 1
  end
  
  def after(state)
    @examples += 1
    state.exceptions.each do |msg, exc|
      state.failure?(exc) ? @failures += 1 : @errors += 1
    end
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
