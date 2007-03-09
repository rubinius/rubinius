require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "a Specification declared with {:should_raise => " do
  setup do
    @reporter = mock("reporter")
    @reporter.stub!(:spec_started)
  end
  
  def verify_error(error, message=nil)
    error.should_be_an_instance_of Spec::Expectations::ExpectationNotMetError
    unless message.nil?
      if message.is_a?(Regexp)
        error.message.should =~ message
      else
        error.message.should == message
      end
    end
  end

  specify "true} should pass when there is an ExpectationNotMetError" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => true) do
      raise Spec::Expectations::ExpectationNotMetError
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      error.should be(nil)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "true} should fail if nothing is raised" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => true) {}
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error, /specify block expected Exception but nothing was raised/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "NameError} should pass when there is a NameError" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => NameError) do
      raise NameError
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      error.should be(nil)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "NameError} should fail when there is no error" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => NameError) do
      #do nothing
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error,/specify block expected NameError but nothing was raised/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "NameError} should fail when there is the wrong error" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => NameError) do
      raise RuntimeError
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error, /specify block expected NameError but raised.+RuntimeError/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError]} should pass when there is a NameError" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError]) do
      raise NameError
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      error.should be(nil)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError]} should fail when there is no error" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError]) do
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error, /specify block expected NameError but nothing was raised/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError]} should fail when there is the wrong error" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError]) do
      raise RuntimeError
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error, /specify block expected NameError but raised.+RuntimeError/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError, 'message'} should pass when there is a NameError with the right message" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError, 'expected']) do
      raise NameError, 'expected'
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      error.should be(nil)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError, 'message'} should pass when there is a NameError with a message matching a regex" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError, /xpec/]) do
      raise NameError, 'expected'
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      error.should be(nil)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError, 'message'} should fail when there is a NameError with the wrong message" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError, 'expected']) do
      raise NameError, 'wrong message'
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error, /specify block expected #<NameError: expected> but raised #<NameError: wrong message>/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end

  specify "[NameError, 'message'} should fail when there is a NameError with a message not matching regexp" do
    spec = Spec::Runner::Specification.new("spec", :should_raise => [NameError, /exp/]) do
      raise NameError, 'wrong message'
    end
    @reporter.should_receive(:spec_finished) do |spec_name, error|
      verify_error(error, /specify block expected #<NameError: \(\?-mix:exp\)> but raised #<NameError: wrong message>/)
    end
    spec.run(@reporter, nil, nil, nil, nil)
  end
end
