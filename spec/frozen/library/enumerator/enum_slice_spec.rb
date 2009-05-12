require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require 'enumerator'

describe "Enumerator#enum_slice" do
  ruby_version_is ''...'1.9' do
    it "returns an enumerator of the receiver with iteration of each_slice for each slice of n elements" do
      a = []
      enum = EnumSpecs::Numerous.new.enum_slice(4)
      enum.should be_kind_of(enumerator_class)
      enum.each { |e| a << e }
      a.should == [[2, 5, 3, 6], [1, 4]]
    end  
  end
end