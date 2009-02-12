require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/stringsymboladapter'

class StringSymbolSpecs
  include StringSymbolAdapter
end

describe StringSymbolAdapter, "#convert_name" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  after :all do
    $VERBOSE = @verbose
  end

  before :each do
    @ruby_version = Object.const_get :RUBY_VERSION

    @name = mock("name")
  end

  after :each do
    Object.const_set :RUBY_VERSION, @ruby_version
  end

  it "converts the name to a string if RUBY_VERSION < 1.9" do
    Object.const_set :RUBY_VERSION, "1.8.6"
    @name.should_receive(:to_s).and_return("method_name")
    StringSymbolSpecs.new.convert_name @name
  end

  it "does not convert the name to a string if RUBY_VERSION >= 1.9" do
    Object.const_set :RUBY_VERSION, "1.9.0"
    @name.should_not_receive(:to_s)
    StringSymbolSpecs.new.convert_name @name
  end
end
