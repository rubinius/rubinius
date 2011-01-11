require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/enumerator_class'
require 'mspec/guards'

describe "#enumerator_class" do

  ruby_version_is ''...'1.9' do
    it "returns Enumerable::Enumerator in Ruby 1.8.6-" do
      require 'enumerator'
      enumerator_class.should == Enumerable::Enumerator
    end
  end

  ruby_version_is '1.9' do
    it "returns Enumerator in Ruby 1.8.7+" do
      enumerator_class.should == Enumerator
    end
  end
end
