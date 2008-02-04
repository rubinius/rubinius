require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "BaseResolver#resolve" do
  before(:all) do
    @br = RbYAML::BaseResolver.new
    @implicit = [true, false]
  end

  it "resolves str" do
    @br.resolve(RbYAML::ScalarNode, "username", @implicit).should == 'tag:yaml.org,2002:str'
  end

  it "resolves sym" do
    @br.resolve(RbYAML::ScalarNode, ":username", @implicit).should == 'tag:yaml.org,2002:sym'
  end

  it "resolves bool" do
    @br.resolve(RbYAML::ScalarNode, "true", @implicit).should == 'tag:yaml.org,2002:bool'
    @br.resolve(RbYAML::ScalarNode, "false", @implicit).should == 'tag:yaml.org,2002:bool'
  end

  it "resolves float" do
    @br.resolve(RbYAML::ScalarNode, "799.90", @implicit).should == 'tag:yaml.org,2002:float'
  end

  it "resolves int" do
    # Fixnum
    @br.resolve(RbYAML::ScalarNode, "800", @implicit).should == 'tag:yaml.org,2002:int'
    # Bignum
    @br.resolve(RbYAML::ScalarNode, "599999999", @implicit).should == 'tag:yaml.org,2002:int'
  end

  it "resolves merge" do
    @br.resolve(RbYAML::ScalarNode, "<<", @implicit).should == 'tag:yaml.org,2002:merge'
  end

  it "resolves null" do
    @br.resolve(RbYAML::ScalarNode, "", @implicit).should == 'tag:yaml.org,2002:null'
    @br.resolve(RbYAML::ScalarNode, "~", @implicit).should == 'tag:yaml.org,2002:null'
    @br.resolve(RbYAML::ScalarNode, "null", @implicit).should == 'tag:yaml.org,2002:null'
  end

  it "resolves timestamp" do
    @br.resolve(RbYAML::ScalarNode, "2001-07-02", @implicit).should == 'tag:yaml.org,2002:timestamp'
  end

  it "resolves value" do
    @br.resolve(RbYAML::ScalarNode, "=", @implicit).should == 'tag:yaml.org,2002:value'
  end
end
