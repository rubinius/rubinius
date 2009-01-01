require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Cookie#value" do
  it "returns self's value" do
    cookie = CGI::Cookie.new("test-cookie")
    cookie.value.should == []

    cookie = CGI::Cookie.new("test-cookie", "one")
    cookie.value.should == ["one"]
    
    cookie = CGI::Cookie.new("test-cookie", "one", "two", "three")
    cookie.value.should == ["one", "two", "three"]
    
    cookie = CGI::Cookie.new("name" => "test-cookie", "value" => ["one", "two", "three"])
    cookie.value.should == ["one", "two", "three"]
  end
end

describe "CGI::Cookie#value=" do
  before(:each) do
    @cookie = CGI::Cookie.new("test-cookie")
  end
  
  it "sets self's value" do
    @cookie.value = ["one"]
    @cookie.value.should == ["one"]
    
    @cookie.value = ["one", "two", "three"]
    @cookie.value.should == ["one", "two", "three"]
  end
  
  ruby_bug "http://redmine.ruby-lang.org/issues/show/229", "1.8.7" do
    it "automatically converts the passed Object to an Array using #Array" do
      @cookie.value = "test"
      @cookie.value.should == ["test"]
      
      obj = mock("to_a")
      obj.should_receive(:to_a).and_return(["1", "2"])
      @cookie.value = obj
      @cookie.value.should == ["1", "2"]

      obj = mock("to_ary")
      obj.should_receive(:to_ary).and_return(["1", "2"])
      @cookie.value = obj
      @cookie.value.should == ["1", "2"]
    end
    
    it "does keep self and the values in sync" do
      @cookie.value = ["one", "two", "three"]
      @cookie[0].should == "one"
      @cookie[1].should == "two"
      @cookie[2].should == "three"
    end
  end
end
