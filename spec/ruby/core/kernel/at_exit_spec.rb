require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.at_exit" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:at_exit)
  end

  it "runs after all other code" do
    ruby_exe("at_exit {print 5}; print 6").should == "65"
  end

  it "runs in reverse order of registration" do
    code = "at_exit {print 4};at_exit {print 5}; print 6; at_exit {print 7}"
    ruby_exe(code).should == "6754"
  end

  it "allows calling exit inside at_exit handler" do
    code = "at_exit {print 3}; at_exit {print 4; exit; print 5}; at_exit {print 6}"
    ruby_exe(code).should == "643"
  end

  it "gives access to the last raised exception" do
    code = 'at_exit{ puts $! == $exception }; begin; raise "foo"; rescue => e; $exception = e; raise; end'
    # The true is embedded in the stack trace of the uncaught exception
    ruby_exe("STDERR=STDOUT; #{code}", :escape => true).should =~ /true/
  end

end

describe "Kernel#at_exit" do
  it "needs to be reviewed for spec completeness"
end
