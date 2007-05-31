require File.dirname(__FILE__) + '/../spec_helper'

# Currently extra constants
describe 'The set of pre-defined constants' do  
  it "includes RUBY_ENGINE" do
    Object.const_defined?(:RUBY_ENGINE).should == true
  end
end
