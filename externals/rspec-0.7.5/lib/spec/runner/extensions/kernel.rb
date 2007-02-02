require 'spec/runner/extensions/object'

module Kernel
  def context(name, &block)
    context = Spec::Runner::Context.new(name, &block)
    context_runner.add_context(context)
  end
  
private
  
  def context_runner
    if $context_runner.nil?; $context_runner = ::Spec::Runner::OptionParser.new.create_context_runner(ARGV.dup, STDERR, STDOUT, false); at_exit { $context_runner.run(false) }; end
    $context_runner
  end
end
