require File.dirname(__FILE__) + '/../spec_helper'

describe "The ensure keyword" do
  it "should execute as a result of a throw within it's block" do
    i = []
    begin
      begin
        i << :begin
        raise "blah"
      ensure
        i << :ensure
      end
    rescue RuntimeError
    end
    i.should == [:begin,:ensure]
  end  
end
