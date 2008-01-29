require 'mspec/expectations'

class TallyAction
  attr_reader :files, :examples, :expectations, :failures, :errors
  
  def initialize
    @files = @examples = @expectations = @failures = @errors = 0
  end
  
  def register
    MSpec.register :load, self
    MSpec.register :after, self
    MSpec.register :expectations, self
  end
  
  def load
    @files += 1
  end
  
  def expectation
    @expectations += 1
  end
  
  def after(state)
    @examples += 1
    state.exceptions.each do |e|
      state.failure?(e) ? @failures += 1 : @errors += 1
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
