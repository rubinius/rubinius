require File.dirname(__FILE__) + '/../../spec_helper'

@exception_new = shared "Exception.new" do |cmd|
  describe "Exception.#{cmd}" do
    it "creates a new instance of Exception" do
      Exception.should_be_ancestor_of(Exception.send(cmd).class)
    end

    it "sets the message of the Exception when passes a message" do
      Exception.send(cmd, "I'm broken.").message.should == "I'm broken."
    end

    it "returns 'Exception' for message when no message given" do
      Exception.send(cmd).message.should == "Exception"
    end
  end
end

describe "Exception.new" do
  it_behaves_like(@exception_new, :new)
end
