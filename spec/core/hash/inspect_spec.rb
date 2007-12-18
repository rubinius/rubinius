require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#inspect" do
  it "returns a string representation with same order as each()" do
    h = {:a => [1, 2], :b => -2, :d => -6, nil => nil}
    
    pairs = []
    h.each do |key, value|
      pairs << key.inspect + "=>" + value.inspect
    end
    
    str = '{' + pairs.join(', ') + '}'
    h.inspect.should == str
  end

  it "calls inspect on keys and values" do
    key = mock('key')
    val = mock('val')
    key.should_receive(:inspect).and_return('key')
    val.should_receive(:inspect).and_return('val')
    
    { key => val }.inspect.should == '{key=>val}'
  end

  it "handles recursive hashes" do
    x = {}
    x[0] = x
    x.inspect.should == '{0=>{...}}'

    x = {}
    x[x] = 0
    x.inspect.should == '{{...}=>0}'

    x = {}
    x[x] = x
    x.inspect.should == '{{...}=>{...}}'

    x = {}
    y = {}
    x[0] = y
    y[1] = x
    x.inspect.should == "{0=>{1=>{...}}}"
    y.inspect.should == "{1=>{0=>{...}}}"

    x = {}
    y = {}
    x[y] = 0
    y[x] = 1
    x.inspect.should == "{{{...}=>1}=>0}"
    y.inspect.should == "{{{...}=>0}=>1}"
    
    x = {}
    y = {}
    x[y] = x
    y[x] = y
    x.inspect.should == "{{{...}=>{...}}=>{...}}"
    y.inspect.should == "{{{...}=>{...}}=>{...}}"
  end
end
