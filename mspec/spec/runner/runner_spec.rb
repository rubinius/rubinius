require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/runner'
require File.dirname(__FILE__) + '/../../runner/formatters/dotted'
require File.dirname(__FILE__) + '/../../runner/formatters/ci'

describe SpecRunner do
  before :each do
    module Mock; end
    Mock.stub!(:verify_count)
    Mock.stub!(:cleanup)
    
    @runner = SpecRunner.new
    @runner.instance_variable_set(:@stack, [DescribeState.new])
  end
  
  it "accepts a formatter when creating an instance" do
    SpecRunner.new(CIFormatter.new)
  end
  
  it "defaults to the DottedFormatter if one is not passed to new" do
    @runner.formatter.should be_an_instance_of(DottedFormatter)
  end
  
  it "provides a getter for 'formatter'" do
    @runner.formatter.should be_an_instance_of(DottedFormatter)
  end
  
  it "provides a setter for 'formatter'" do
    @runner.formatter = CIFormatter.new
    @runner.formatter.should be_an_instance_of(CIFormatter)
  end

  it "provides #escape that converts its argument to an escaped Regexp" do
    @runner.escape('[me]').should == /\[me\]/
  end
  
  it "provides #escape that returns its argument unmodified if passed an instance of a Regexp" do
    re = /me/
    @runner.escape(re).should equal(re)
  end

  it "provides #convert_to_regexps that converts a list of items to escaped Regexps" do
    @runner.convert_to_regexps('1', '[m]', '?:e').should == [/1/, /\[m\]/, /\?:e/]
  end

  it "provides #only that converts a list of items to escaped Regexps" do
    @runner.only('a', 'b', 'c').should == [/a/, /b/, /c/]
  end

  it "provides #except that converts a list of items to escaped Regexps" do
    @runner.except('a', 'b', 'c').should == [/a/, /b/, /c/]
  end
  
  it "provides #skip? that returns true if current spec full message matches an except element" do
    @runner.except('this one')
    @runner.formatter.before_describe('this')
    @runner.formatter.before_it('one')
    @runner.skip?.should == true
  end
  
  it "provides #before that accepts :each and a block" do
    @runner.before(:each) { }
  end
  
  it "provides #before that accepts :all and a block" do
    @runner.before(:all) { }
  end
  
  it "provides #after that accepts :each and a block" do
    @runner.after(:each) { }
  end
  
  it "provides #after that accepts :all and a block" do
    @runner.after(:all) { }
  end
  
  it "provides #it that accepts two arguments and a block" do
    @runner.it("") { }
  end
  
  it "provides #describe that accepts two arguments and a block" do
    @runner.describe(Object, "") { }
  end
end

describe SpecRunner, "complete run" do
  before :all do
    module Mock; end
    Mock.stub!(:verify_count)
    Mock.stub!(:cleanup)
    
    $runner_spec_runner = SpecRunner.new
    @report = CaptureOutput.new
    $runner_spec_runner.formatter = DottedFormatter.new(@report)
    $runner_spec_runner.formatter.stub!(:print_time)
  end    
  
  it "provides #describe that yields to its block and calls all before, after, and it blocks" do
    run = []
    $runner_spec_runner.describe("") do
      $runner_spec_runner.before(:all)  { run << "before :all" }
      $runner_spec_runner.before(:each) { run << "  before :each" }
      $runner_spec_runner.after(:each)  { run << "  after :each" }
      $runner_spec_runner.after(:all)   { run << "after :all" }
      $runner_spec_runner.it("#first")  { run << "  first" }
      $runner_spec_runner.it("#second") { run << "  second" }
    end
    run.join.should == 
      "before :all" \
      "  before :each" \
      "  first" \
      "  after :each" \
      "  before :each" \
      "  second" \
      "  after :each" \
      "after :all"
    $runner_spec_runner.formatter.summary
    @report.should == "..\n\n2 examples, 0 failures\n"
  end
end

describe DescribeState do
  before :each do
    @state = DescribeState.new
  end
  
  it "provides #before_all" do
    @state.before_all.should be_an_instance_of(Array)
  end

  it "provides #before_each" do
    @state.before_each.should be_an_instance_of(Array)
  end

  it "provides #after_each" do
    @state.after_each.should be_an_instance_of(Array)
  end

  it "provides #after_all" do
    @state.after_all.should be_an_instance_of(Array)
  end
  
  it "provides #it" do
    @state.it.should be_an_instance_of(Array)
  end
end
