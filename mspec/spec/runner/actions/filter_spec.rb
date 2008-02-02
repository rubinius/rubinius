require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/actions/filter'
require File.dirname(__FILE__) + '/../../../runner/mspec'
require File.dirname(__FILE__) + '/../../../runner/tag'

describe ActionFilter do
  before :each do
    @tag = SpecTag.new "tag(comment):description"
  end
  
  it "creates a filter from a single tag" do
    MSpec.should_receive(:read_tags).with("tag").and_return([@tag])
    MatchFilter.should_receive(:new).with(nil, "description")
    ActionFilter.new("tag", nil)
  end
  
  it "creates a filter from an array of tags" do
    MSpec.should_receive(:read_tags).with("tag", "key").and_return([@tag])
    MatchFilter.should_receive(:new).with(nil, "description")
    ActionFilter.new(["tag", "key"], nil)
  end
  
  it "creates a filter from a single description" do
    MSpec.should_receive(:read_tags).and_return([])
    MatchFilter.should_receive(:new).with(nil, "match me")
    ActionFilter.new(nil, "match me")
  end
  
  it "creates a filter from an array of descriptions" do
    MSpec.should_receive(:read_tags).and_return([])
    MatchFilter.should_receive(:new).with(nil, "match me", "again")
    ActionFilter.new(nil, ["match me", "again"])
  end
  
  it "creates a filter from both tags and descriptions" do
    MSpec.should_receive(:read_tags).and_return([@tag])
    MatchFilter.should_receive(:new).with(nil, "match me", "again", "description")
    ActionFilter.new("tag", ["match me", "again"])
  end
end

describe ActionFilter, "#===" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @action = ActionFilter.new(nil, ["catch", "if you"])
  end
  
  it "returns true if the argument matches any of the descriptions" do
    @action.===("catch").should == true
    @action.===("if you can").should == true
  end
  
  it "returns false if the argument does not match any of the descriptions" do
    @action.===("match me").should == false
    @action.===("if I can").should == false
  end
end
