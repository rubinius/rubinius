require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/language_version'

describe Object, "#language_version" do
  before :all do
    @ruby_version = Object.const_get :RUBY_VERSION

    Object.const_set :RUBY_VERSION, "8.2.3"

    dir = File.dirname(File.expand_path(__FILE__))
    @name = "#{dir}/versions/method_8.2.rb"
  end

  after :all do
    Object.const_set :RUBY_VERSION, @ruby_version
  end

  it "loads files conditionally based on name and RUBY_VERSION if it exists" do
    File.should_receive(:exists?).with(@name).and_return(true)
    should_receive(:require).with(@name)
    language_version __FILE__, "method"
  end

  it "does not load the file if it does not exist" do
    File.should_receive(:exists?).with(@name).and_return(false)
    should_not_receive(:require).with(@name)
    language_version __FILE__, "method"
  end
end
