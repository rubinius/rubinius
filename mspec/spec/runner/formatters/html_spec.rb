require File.dirname(__FILE__) + '/../../spec_helper'
require 'mspec/guards/guard'
require 'mspec/runner/formatters/html'
require 'mspec/runner/mspec'
require 'mspec/runner/state'

describe HtmlFormatter do
  before :each do
    @formatter = HtmlFormatter.new
  end

  it "responds to #register by registering itself with MSpec for appropriate actions" do
    MSpec.stub!(:register)
    MSpec.should_receive(:register).with(:start, @formatter)
    MSpec.should_receive(:register).with(:enter, @formatter)
    MSpec.should_receive(:register).with(:leave, @formatter)
    @formatter.register
  end
end

describe HtmlFormatter, "#start" do
  before :each do
    $stdout = @out = IOStub.new
    @formatter = HtmlFormatter.new
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints the HTML head" do
    @formatter.start
    @out.should ==
%[<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
    "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>Spec Output For ruby (1.8.6)</title>
<style type="text/css">
ul {
  list-style: none;
}
.fail {
  color: red;
}
.pass {
  color: green;
}
#details :target {
  background-color: #ffffe0;
}
</style>
</head>
<body>
]
  end
end

describe HtmlFormatter, "#enter" do
  before :each do
    $stdout = @out = IOStub.new
    @formatter = HtmlFormatter.new
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints the #describe string" do
    @formatter.enter "describe"
    @out.should == "<div><p>describe</p>\n<ul>\n"
  end
end

describe HtmlFormatter, "#leave" do
  before :each do
    $stdout = @out = IOStub.new
    @formatter = HtmlFormatter.new
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints the closing tags for the #describe string" do
    @formatter.leave
    @out.should == "</ul>\n</div>\n"
  end
end

describe HtmlFormatter, "#after" do
  before :each do
    $stdout = @out = IOStub.new
    @formatter = HtmlFormatter.new
    @state = SpecState.new("describe", "it")
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints the #it once when there are no exceptions raised" do
    @formatter.after @state
    @out.should == %[<li class="pass">- it</li>\n]
  end

  it "prints the #it string once for each exception raised" do
    @formatter.register
    @state.exceptions << ["msg", ExpectationNotMetError.new("disappointing")]
    @state.exceptions << ["msg", MSpecExampleError.new("painful")]
    @formatter.tally.after @state
    @formatter.after @state
    @out.should == 
%[<li class="fail">- it (<a href="#details-1">FAILED - 1</a>)</li>
<li class="fail">- it (<a href="#details-2">ERROR - 2</a>)</li>
]
  end
end

describe HtmlFormatter, "#finish" do
  before :each do
    @tally = mock("tally", :null_object => true)
    TallyAction.stub!(:new).and_return(@tally)
    @timer = mock("timer", :null_object => true)
    TimerAction.stub!(:new).and_return(@timer)

    $stdout = @out = IOStub.new
    @state = SpecState.new("describe", "it")
    MSpec.stub!(:register)
    @formatter = HtmlFormatter.new
    @formatter.register
    @exception = MSpecExampleError.new("broken")
    @exception.stub!(:backtrace).and_return(["file.rb:1", "file.rb:2"])
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints a failure message for an exception" do
    @state.exceptions << ["msg", @exception]
    @formatter.instance_variable_set :@states, [@state]
    @formatter.finish
    @out.should =~ %r[<p>describe it ERROR</p>]
  end

  it "prints a backtrace for an exception" do
    @formatter.stub!(:backtrace).and_return("path/to/some/file.rb:35:in method")
    @state.exceptions << ["msg", @exception]
    @formatter.instance_variable_set :@states, [@state]
    @formatter.finish
    @out.should =~ %r[<pre>.*path/to/some/file.rb:35:in method.*</pre>]m
  end

  it "prints a summary of elapsed time" do
    @timer.should_receive(:format).and_return("Finished in 2.0 seconds")
    @formatter.finish
    @out.should =~ %r[<p>Finished in 2.0 seconds</p>\n]
  end

  it "prints a tally of counts" do
    @tally.should_receive(:format).and_return("1 example, 0 failures")
    @formatter.finish
    @out.should =~ %r[<p class="pass">1 example, 0 failures</p>]
  end

  it "prints errors, backtraces, elapsed time, and tallies" do
    @state.exceptions << ["msg", @exception]
    @formatter.stub!(:backtrace).and_return("path/to/some/file.rb:35:in method")
    @timer.should_receive(:format).and_return("Finished in 2.0 seconds")
    @tally.should_receive(:format).and_return("1 example, 1 failures")
    @formatter.instance_variable_set :@states, [@state]
    @formatter.finish
    @out.should ==
%[<hr>
<ol id="details">
<li id="details-1"><p>describe it ERROR</p>
<p>MSpecExampleError: broken</p>
<pre>
path/to/some/file.rb:35:in method</pre>
</li>
</ol>
<p>Finished in 2.0 seconds</p>
<p class="fail">1 example, 1 failures</p>
</body>
</html>
]
  end
end
