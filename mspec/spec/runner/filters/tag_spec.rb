require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/mspec'
require File.dirname(__FILE__) + '/../../../runner/filters/match'
require File.dirname(__FILE__) + '/../../../runner/filters/tag'

describe TagFilter, "#load" do
  before :each do
    @filter = TagFilter.new :include, "tag", "key"
    @tag = SpecTag.new "tag(comment):description"
    MSpec.stub!(:read_tags).and_return([@tag])
  end
  
  it "loads tags from the tag file" do
    MSpec.should_receive(:read_tags).with("tag", "key").and_return([])
    @filter.load
  end
  
  it "creates a MatchFilter from the descriptions matching the tags" do
    match = mock("match filter", :null_object => true)
    MatchFilter.should_receive(:new).with(:include, "description").and_return(match)
    @filter.load
  end
  
  it "does not create a MatchFilter if no tags were found" do
    MSpec.should_receive(:read_tags).and_return([])
    MatchFilter.should_not_receive(:new)
    @filter.load
  end
  
  it "registers the MatchFilter if there were tags found in the tag file" do
    match = mock("match filter")
    match.should_receive(:register)
    MatchFilter.should_receive(:new).with(:include, "description").and_return(match)
    @filter.load
  end
  
  it "does not register the MatchFilter if no tags were found" do
    MSpec.should_receive(:read_tags).and_return([])
    MSpec.should_not_receive(:register)
    @filter.load
  end
end

describe TagFilter, "#unload" do
  before :each do
    @filter = TagFilter.new :include, "tag", "key"
    @tag = SpecTag.new "tag(comment):description"
    MSpec.stub!(:read_tags).and_return([@tag])
  end
  
  it "unregisters the MatchFilter if one was registered" do
    match = mock("match filter", :null_object => true)
    match.should_receive(:unregister)
    MatchFilter.stub!(:new).with(:include, "description").and_return(match)
    @filter.load
    @filter.unload
  end
  
  it "does not unregister the MatchFilter if none was created" do
    MSpec.should_receive(:read_tags).and_return([])
    MSpec.should_not_receive(:unregister)
    @filter.load
    @filter.unload
  end
end

describe TagFilter, "#register" do
  it "registers itself with MSpec for the :load, :unload actions" do
    filter = TagFilter.new(nil)
    MSpec.should_receive(:register).with(:load, filter)
    MSpec.should_receive(:register).with(:unload, filter)
    filter.register
  end
end

describe TagFilter, "#unregister" do
  it "unregisters itself with MSpec for the :load, :unload actions" do
    filter = TagFilter.new(nil)
    MSpec.should_receive(:unregister).with(:load, filter)
    MSpec.should_receive(:unregister).with(:unload, filter)
    filter.unregister
  end
end
