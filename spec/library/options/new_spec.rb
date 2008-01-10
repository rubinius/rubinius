require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/string'
require 'options'

describe "Options.new" do
  it "creates a blank set of options" do
    Options.new.class.should == Options
  end

  it "optionally takes a block that yields the Options instance" do
    oid = nil

    opts = Options.new {|o| oid = o.object_id }

    opts.object_id.should == oid
    opts.class.should == Options
  end

  it "may be given a message to be shown at the top of #usage" do
    o = Options.new 'My very own header'
    o.usage.squish.should == 'Myveryownheader'
  end
end
