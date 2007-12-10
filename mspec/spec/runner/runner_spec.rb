require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/runner'
require File.dirname(__FILE__) + '/../../runner/formatters/dotted'
require File.dirname(__FILE__) + '/../../runner/formatters/ci'

describe SpecRunner do
  before :each do
    @runner = SpecRunner.new
  end
  
  it "accepts a formatter when creating an instance" do
    SpecRunner.new(CIFormatter.new)
  end
  
  it "attempts to get the desired formatter from the environment if one is not passed to new" do
    ENV['SPEC_FORMATTER'] = 'CIFormatter'
    SpecRunner.new.formatter.should be_an_instance_of(CIFormatter)
    ENV.delete 'SPEC_FORMATTER'
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
    @runner.it(Object, "") { }
  end
  
  it "provides #describe that accepts two arguments and a block" do
    @runner.describe(Object, "") { }
  end
  
  it "provides #describe that yields to its block and calls all before, after, and it blocks" do
    module Mock; end
    Mock.stub!(:verify_count)
    Mock.stub!(:cleanup)
    
    run = []
    @report = CaptureOutput.new
    @runner.formatter = DottedFormatter.new(@report)
    @runner.describe(Object, "") do
      @runner.before(:all)  { run << "before :all" }
      @runner.before(:each) { run << "  before :each" }
      @runner.after(:each)  { run << "  after :each" }
      @runner.after(:all)   { run << "after :all" }
      @runner.it(Object, "#first")  { run << "  first" }
      @runner.it(Object, "#second") { run << "  second" }
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
    @runner.formatter.summary
    @report.should == "..\n\n2 examples, 0 failures\n"
  end
end
