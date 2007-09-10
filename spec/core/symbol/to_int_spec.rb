require File.dirname(__FILE__) + '/../../spec_helper'

describe "Symbol#to_int" do
  it "returns Symbol#to_i" do
    begin
      old_verbose, $VERBOSE = $VERBOSE, nil
      
      :ruby.to_int.is_a?(Integer).should == true
      :ruby.to_int.should == :ruby.to_i
      :ruby.to_int.should_not == :rubinius.to_i
    ensure
      $VERBOSE = old_verbose
    end
  end
  
  it "raises a warning" do
    begin
      new_stderr = Object.new
      class << new_stderr
        attr_reader :output
        def write(str) (@output ||= "") << str end
      end
        
      old_verbose, $VERBOSE = $VERBOSE, true    
      old_stderr,  $stderr  = $stderr,  new_stderr

      :ruby.to_int
      $stderr.output.split(": ").last.should == "treating Symbol as an integer\n"
    ensure
      $stderr, $VERBOSE = old_stderr, old_verbose
    end
  end
end
