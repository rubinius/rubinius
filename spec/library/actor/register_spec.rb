require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor registration" do
  it "stores actors by identifier" do
    actor = Actor.spawn(Actor.current) do |master|
      Actor[:foo] = Actor.current
      msg = Actor.receive
      master.send msg
    end

    Actor[:foo].should == actor

    actor.send "test"
    msg = Actor.receive
    msg.should == "test"

    Actor[:foo] = nil
    Actor[:foo].should == nil
  end
end
