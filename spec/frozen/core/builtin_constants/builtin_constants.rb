require File.dirname(__FILE__) + '/../../spec_helper'

describe "RUBY_VERSION" do
	it "is a String" do
		RUBY_VERSION.should be_kind_of(String)
	end
end

describe "RUBY_PATCHLEVEL" do
	it "is a Fixnum" do
		RUBY_PATCHLEVEL.should be_kind_of(Fixnum)
	end
end