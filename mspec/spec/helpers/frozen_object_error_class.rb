require 'spec_helper'
require 'mspec/guards'
require 'mspec/helpers'

describe "#frozen_object_error_class" do
  ruby_version_is ''...'1.9' do
    it "is TypeError" do
      frozen_object_error_class.should == TypeError
    end
  end

  ruby_version_is '1.9' do
    it "is RuntimeError" do
      frozen_object_error_class.should == RuntimeError
    end
  end
end
