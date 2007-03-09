require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "ExecutionContext" do
  
  specify "should provide duck type" do
    dt = duck_type(:length)
    dt.should be_an_instance_of(Spec::Mocks::DuckTypeArgConstraint)
    dt.matches?([]).should be_true
  end

  specify "should violate on :violated" do
    lambda do
      Spec::Runner::ExecutionContext.new(nil).violated
    end.should raise_error(Spec::Expectations::ExpectationNotMetError)
  end

  specify "should provide mock" do
    mock("thing").should be_an_instance_of(Spec::Mocks::Mock)
  end

  # TODO - the following two specs don't work when combined:
  # specify "should provide stub" do
  #   thing_stub = stub("thing", :a => "A", :b => "B").should be_an_instance_of(Spec::Mocks::Mock)
  #   thing_stub.a.should == "A"
  #   thing_stub.b.should == "B"
  # end
  # 
  # TypeError in 'ExecutionContext should provide stub'
  # ["a?", "as?"] is not a symbol
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/expectations/should/should.rb:66:in `send'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/expectations/should/should.rb:66:in `__delegate_method_missing_to_target'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/expectations/should/base.rb:53:in `method_missing'
  # ./spec/spec/runner/execution_context_spec.rb:27:
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/specification.rb:55:in `execute_spec'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/specification.rb:30:in `run'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/context.rb:57:in `run'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/context.rb:54:in `run'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/context_runner.rb:23:in `run'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/context_runner.rb:22:in `run'
  # /Users/david/projects/ruby/rspec/trunk/rspec/lib/spec/runner/command_line.rb:27:in `run'
  # bin/spec:4:
  specify "should provide stub" do
    thing_stub = stub("thing", :a => "A", :b => "B").should be_an_instance_of(Spec::Mocks::Mock)
  end
  
  specify "should add method stubs to stub" do
    thing_stub = stub("thing", :a => "A", :b => "B")
    thing_stub.a.should == "A"
    thing_stub.b.should == "B"
  end

end
