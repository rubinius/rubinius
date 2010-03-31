require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/lambda', __FILE__)

# The functionality of lambdas is specified in core/proc

describe "Kernel.lambda" do
  it_behaves_like(:kernel_lambda, :lambda)
  it_behaves_like(:kernel_lambda_return_like_method, :lambda)

  it "is a private method" do
    Kernel.should have_private_instance_method(:lambda)
  end

  it "does not check the arity of the call when no args are specified" do
    l = lambda { true }
    l.call.should be_true

    # Check one and two args because the block argument handling changes
    # at two arguments.
    l.call(1).should be_true
    l.call(1, 2).should be_true
  end

  it "accepts 0 arguments when used with ||" do
    lambda {
      lambda { || }.call(1)
    }.should raise_error(ArgumentError)
  end

  it "does not check the arity when 1 arg is specified" do
    l = lambda { |a| }

    lambda {
      l.call
    }.should_not raise_error(ArgumentError)

    lambda {
      l.call 1
    }.should_not raise_error(ArgumentError)

    lambda {
      l.call 1, 2
    }.should_not raise_error(ArgumentError)
  end

  it "strictly checks the arity when 0 or 2..inf args are specified" do
    l = lambda { |a,b| }

    lambda {
      l.call
    }.should raise_error(ArgumentError)

    lambda {
      l.call(1)
    }.should raise_error(ArgumentError)

    lambda {
      l.call(1,2)
    }.should_not raise_error(ArgumentError)
  end

end

