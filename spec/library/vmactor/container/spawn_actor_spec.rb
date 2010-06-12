require File.expand_path('../../../../spec_helper', __FILE__)
require 'vmactor'

describe "VMActor::Container#spawn_actor" do
  before :each do
    @container = VMActor::Container.new "-rspec/library/fixtures/pingpong"
  end

  it "creates Actors on remote VMs" do
    actor = @container.spawn_actor :Kernel, :puts, 'hi'
    actor.should be_kind_of(VMActor)
  end

  it "allows Actors in different VMs to send messages to each other" do
    actor = @container.spawn_actor :Pingpong, :run
    actor << Rubinius::Tuple[:ping, Actor.current]

    Actor.receive do |filter|
      filter.when(:pong) { |msg| msg }
    end.should == :pong
  end

  it "evaluates strings passed to Container#spawn_actor in remote actor scope" do
    actor = @container.spawn_actor "Pingpong.run"
    actor << Rubinius::Tuple[:ping, Actor.current]

    Actor.receive do |filter|
      filter.when(:pong) { |msg| msg }
    end.should == :pong
  end
end
