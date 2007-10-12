require File.dirname(__FILE__) + '/../spec_helper'

describe "The ensure keyword" do
  it "should execute as a result of a throw within it's block" do
    i = []
    catch(:exit) do
      begin
        i << :begin
        throw :exit
      ensure
        i << :ensure
      end      
    end
    i.should == [:begin,:ensure]
  end  
end
