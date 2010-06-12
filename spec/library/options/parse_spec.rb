require File.expand_path('../../../spec_helper', __FILE__)
require 'options'

describe "Options#parse" do
  before do
    @o = Options.new

    @o.option '-b --bb Bb'
    @o.option '-c --cc Cc'
    @o.option '-d --dd Dd'
    @o.option '-n --none None'
    @o.option '-o --one  One',  :one
    @o.option '-m --many Many', :many
    @o.option '-a --any  Any',  :any
  end

  it "raises an ArgumentError by default" do
    lambda { Options.new.parse '-h' }.should raise_error(ArgumentError)
  end

  it "allows overriding of default by supplying a block to #on_error" do
    o = Options.new {|o| o.on_error { :error } }
    o.parse('-h').should == :error
  end

  it "supplies the #on_error block with the Options object and the exception" do
    o = Options.new {|o| o.on_error {|op, ex| [op.class, ex.class] } }
    o.parse('-h').should == [Options, ArgumentError]
  end

  it "raises an error if given empty input" do
    lambda { @o.parse '' }.should raise_error(ArgumentError)
    lambda { @o.parse [] }.should raise_error(ArgumentError)
  end

  it "returns a Hash with given options as defined keys" do
    h = @o.parse '--dd'

    h.key?('dd').nil?.should == false
  end

  it "makes given option available both as long and short version" do
    h = @o.parse '--dd'

    h.key?('d').nil?.should == false
    h.key?('dd').nil?.should == false
  end

  it "sets the value of any given option without a parameter to true" do
    h = @o.parse '--dd'

    h.key?('d').should == true
    h.key?('dd').should == true
  end

  it "places any given arguments in :args if they do not belong to options" do
    h = @o.parse '-d ARG'

    h[:args].should == ['ARG']
  end

  it "takes short options separately" do
    h = @o.parse '-b -c -d'

    h.key?('b').should == true
    h.key?('bb').should == true
    h.key?('c').should == true
    h.key?('cc').should == true
    h.key?('d').should == true
    h.key?('dd').should == true
  end

  it "takes short options combined" do
    h = @o.parse '-bcd'

    h.key?('b').should == true
    h.key?('bb').should == true
    h.key?('c').should == true
    h.key?('cc').should == true
    h.key?('d').should == true
    h.key?('dd').should == true
  end

  it "takes short options interspersed with nonoption-arguments" do
    h = @o.parse '-bd ARG -c'

    h.key?('d').should == true
    h.key?('dd').should == true
    h.key?('b').should == true
    h.key?('bb').should == true
    h.key?('c').should == true
    h.key?('cc').should == true
    h[:args].should == ['ARG']
  end

  it "defaults to :none specified arguments which means no following argument is captured" do
    @o.parse('-n ARG')['n'].should == true
    @o.parse('-n ARG')['none'].should == true
  end

  it "stores the argument(s) in an Array stored as the value of the option name" do
    @o.parse('-o ARG')['o'].should == ['ARG']
    @o.parse('-o ARG')['one'].should == ['ARG']
  end

  it "accepts :one to denote a single argument" do
    @o.parse('-o ARG')['o'].should == ['ARG']
    @o.parse('-o ARG')['one'].should == ['ARG']
  end

  it "ignores more than one argument when :one defined, extra go to :args" do
    h = @o.parse '-o ARG1 ARG2'

    h['o'].should == ['ARG1']
    h['one'].should == ['ARG1']
    h[:args].should == ['ARG2']
  end

  it "accepts :many to indicate as many nonoption args as possible until next option" do
    h = @o.parse '-m ARG1 ARG2 ARG3 -n ARG4'

    h['m'].should == %w|ARG1 ARG2 ARG3|
    h['many'].should == %w|ARG1 ARG2 ARG3|
    h['n'].should == true
    h['none'].should == true
    h[:args].should == %w|ARG4|
  end

  it "accepts :any to indicate zero or as many as possible arguments" do
    @o.parse('-a -n')['a'].should == true
    @o.parse('-a -n')['any'].should == true
    @o.parse('-a ARG -n')['a'].should == ['ARG']
    @o.parse('-a ARG -n')['any'].should == ['ARG']
    @o.parse('-a ARG1 ARG2 ARG3 -n')['a'].should == %w|ARG1 ARG2 ARG3|
    @o.parse('-a ARG1 ARG2 ARG3 -n')['any'].should == %w|ARG1 ARG2 ARG3|
  end

  it "fails if :one and no arguments given before next option" do
    lambda { @o.parse '-o'        }.should raise_error(ArgumentError)
    lambda { @o.parse '-o -n ARG' }.should raise_error(ArgumentError)
  end

  it "fails if :many and one or no arguments given" do
    lambda { @o.parse '-m ARG'         }.should raise_error(ArgumentError)
    lambda { @o.parse '-m ARG -n ARG2' }.should raise_error(ArgumentError)
  end

  it "assigns arguments only to the last in a set of combined short options " do
    @o.parse('-nao ARG')['n'].should == true
    @o.parse('-nao ARG')['none'].should == true
    @o.parse('-nao ARG')['a'].should == true
    @o.parse('-nao ARG')['any'].should == true
    @o.parse('-nao ARG')['o'].should == ['ARG']
    @o.parse('-nao ARG')['one'].should == ['ARG']
  end

  it "takes long options separately" do
    h = @o.parse '--bb --cc --dd'

    h.key?('b').should == true
    h.key?('bb').should == true
    h.key?('c').should == true
    h.key?('cc').should == true
    h.key?('d').should == true
    h.key?('dd').should == true
  end

  it "takes long options interspersed with nonoption-arguments" do
    h = @o.parse '--dd ARG --cc'

    h.key?('d').should == true
    h.key?('dd').should == true
    h.key?('c').should == true
    h.key?('cc').should == true
    h[:args].should == ['ARG']
  end

  it "defaults to :none specified arguments which means no following argument is captured" do
    @o.parse('--none ARG')['n'].should == true
    @o.parse('--none ARG')['none'].should == true
  end

  it "stores the argument(s) in an Array stored as the value of the option name" do
    @o.parse('--one ARG')['o'].should == ['ARG']
    @o.parse('--one ARG')['one'].should == ['ARG']
  end

  it "accepts :one to denote a single argument" do
    @o.parse('--one ARG')['o'].should == ['ARG']
    @o.parse('--one ARG')['one'].should == ['ARG']
  end

  it "ignores more than one argument when :one defined" do
    h = @o.parse '--one ARG1 ARG2'

    h['o'].should == ['ARG1']
    h['one'].should == ['ARG1']
    h[:args].should == ['ARG2']
  end

  it "accepts :many to indicate as many nonoption args as follow before the following option" do
    h = @o.parse '--many ARG1 ARG2 ARG3 -n ARG4'

    h['m'].should == %w|ARG1 ARG2 ARG3|
    h['many'].should == %w|ARG1 ARG2 ARG3|
    h['n'].should == true
    h['n'].should == true
    h[:args].should == %w|ARG4|
  end

  it "accepts :any to indicate zero or as many as possible arguments" do
    @o.parse('--any -n')['a'].should == true
    @o.parse('--any -n')['any'].should == true
    @o.parse('--any ARG -n')['a'].should == ['ARG']
    @o.parse('--any ARG -n')['any'].should == ['ARG']
    @o.parse('--any ARG1 ARG2 ARG3 -n')['a'].should == %w|ARG1 ARG2 ARG3|
    @o.parse('--any ARG1 ARG2 ARG3 -n')['any'].should == %w|ARG1 ARG2 ARG3|
  end

  it "fails if :one and no arguments given" do
    lambda { @o.parse '--one'        }.should raise_error(ArgumentError)
    lambda { @o.parse '--one -n ARG' }.should raise_error(ArgumentError)
  end

  it "fails if :many and one or no arguments given" do
    lambda { @o.parse '--many ARG'         }.should raise_error(ArgumentError)
    lambda { @o.parse '--many ARG -n ARG2' }.should raise_error(ArgumentError)
  end
end
