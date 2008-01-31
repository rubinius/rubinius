require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/tag'

describe SpecTag do
  it "accepts an optional string to parse into fields" do
    tag = SpecTag.new "tag(comment):description"
    tag.tag.should == "tag"
    tag.comment.should == "comment"
    tag.description.should == "description"
  end
end

describe SpecTag, "#parse" do
  before :each do
    @tag = SpecTag.new
  end
  
  it "accepts 'tag(comment):description'" do
    @tag.parse "tag(I'm real):Some#method returns a value"
    @tag.tag.should == "tag"
    @tag.comment.should == "I'm real"
    @tag.description.should == "Some#method returns a value"
  end
  
  it "accepts 'tag:description'" do
    @tag.parse "tag:Another#method"
    @tag.tag.should == "tag"
    @tag.comment.should == nil
    @tag.description.should == "Another#method"
  end
  
  it "accepts 'tag():description'" do
    @tag.parse "tag():Another#method"
    @tag.tag.should == "tag"
    @tag.comment.should == nil
    @tag.description.should == "Another#method"
  end
  
  it "ignores '#anything'" do
    @tag.parse "# this could be a comment"
    @tag.tag.should == nil
    @tag.comment.should == nil
    @tag.description.should == nil
  end
end

describe SpecTag, "#to_s" do
  it "formats itself as 'tag(comment):description'" do
    tag = SpecTag.new("tag(comment):description")
    tag.tag.should == "tag"
    tag.comment.should == "comment"
    tag.description.should == "description"
    tag.to_s.should == "tag(comment):description"
  end
  
  it "formats itself as 'tag:description" do
    tag = SpecTag.new("tag:description")
    tag.tag.should == "tag"
    tag.comment.should == nil
    tag.description.should == "description"
    tag.to_s.should == "tag:description"
  end
end
