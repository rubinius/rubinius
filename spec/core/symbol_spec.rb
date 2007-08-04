require File.dirname(__FILE__) + '/../spec_helper'

# ===, id2name, inspect, to_i, to_int, to_s, to_sym

#describe "Symbol.all_symbols" do
#  it "is a pain in the ass to test..."
#end

describe "Symbol#==" do
  it "only returns true when the other is exactly the same symbol" do
    (:ruby == :ruby).should == true
    (:ruby == :"ruby").should == true
    (:ruby == :'ruby').should == true
    (:@ruby == :@ruby).should == true
    
    (:ruby == :@ruby).should == false
    (:foo == :bar).should == false
    (:ruby == 'ruby').should == false
  end
end

symbol_id2name = shared "Symbol#id2name | Symbol#to_s" do |cmd|
  describe "Symbol\##{cmd}" do
    it "returns the string corresponding to self" do
      :rubinius.send(cmd).should == "rubinius"
      :squash.send(cmd).should == "squash"
      :[].send(cmd).should == "[]"
      :@ruby.send(cmd).should == "@ruby"
      :@@ruby.send(cmd).should == "@@ruby"
    end
  end
end

describe "Symbol#id2name" do
  it_behaves_like(symbol_id2name, :id2name)
end

describe "Symbol#inspect" do
  it "returns self as a symbol literal" do
    :fred.inspect.should == ":fred"
    :[].inspect.should == ":[]"
    :@ruby.inspect.should == ":@ruby"
    :@@ruby.inspect.should == ":@@ruby"
  end
end

describe "Symbol#to_i" do
  it "returns an integer that is unique for each symbol for each program execution" do
    :ruby.to_i.is_a?(Integer).should == true
    :ruby.to_i.should == :ruby.to_i
    :ruby.to_i.should_not == :rubinius.to_i
  end
end

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

describe "Symbol#to_s" do
  it_behaves_like(symbol_id2name, :to_s)
end

describe "Symbol#to_sym" do
  it "returns self" do
    [:rubinius, :squash, :[], :@ruby, :@@ruby].each do |sym|
      sym.to_sym.should == sym
    end
  end
end