require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require 'enumerator'

describe "Enumerator#enum_with_index" do
  ruby_version_is ''...'1.9' do
    it "returns an enumerator of the receiver with an iteration of each_with_index" do
      a = []
      enum = EnumSpecs::Numerous.new.enum_with_index
      enum.should be_kind_of(enumerator_class)
      enum.each { |e| a << e }
      a.should == [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]]
    end
  end
end