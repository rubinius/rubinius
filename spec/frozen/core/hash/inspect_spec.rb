require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/to_s'

describe "Hash#inspect" do
  it_behaves_like :to_s, :inspect
 
  # FIXME: When http://redmine.ruby-lang.org/issues/show/1533 is fixed, this
  # block can be removed, as the behaviour will then be shared with to_s
  # completely. 
  it "handles recursive hashes" do
    x = new_hash
    x[0] = x
    x.inspect.should == '{0=>{...}}'

    x = new_hash
    x[x] = 0
    x.inspect.should == '{{...}=>0}'

    x = new_hash
    x[x] = x
    x.inspect.should == '{{...}=>{...}}'

    x = new_hash
    y = new_hash
    x[0] = y
    y[1] = x
    x.inspect.should == "{0=>{1=>{...}}}"
    y.inspect.should == "{1=>{0=>{...}}}"

    x = new_hash
    y = new_hash
    x[y] = 0
    y[x] = 1
    x.inspect.should == "{{{...}=>1}=>0}"
    y.inspect.should == "{{{...}=>0}=>1}"
    
    x = new_hash
    y = new_hash
    x[y] = x
    y[x] = y
    x.inspect.should == "{{{...}=>{...}}=>{...}}"
    y.inspect.should == "{{{...}=>{...}}=>{...}}"
  end
end
