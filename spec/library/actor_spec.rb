require File.dirname(__FILE__) + '/../spec_helper'
require 'actor'

describe "Actor.spawn" do
  before :each do
    @master = Actor.current
  end

  it "creates functioning actors" do
    Data = Struct.new :value
    NUM_ACTORS = 10

    actors = []
    results = []
    NUM_ACTORS.times do 
      actors << Actor.spawn(@master) do |master|
        Actor.receive do |filter|
          filter.when Data do |msg|
            @master.send(msg.value)
          end
        end
      end
    end

    actors.each_with_index do |actor, i|
      actor.send(Data.new(i+1))
    end

    NUM_ACTORS.times { |i| results << Actor.receive }
    results.sort.should == (1..NUM_ACTORS).to_a
  end
end

describe "Actor.register with [identifier, actor]" do
  it "stores actors by identifier" do
    actor = Actor.spawn(Actor.current) do |master|
      Actor.register(:foo, Actor.current)
      msg = Actor.receive
      master.send msg
    end

    Actor.lookup(:foo).should == actor

    actor.send "test"
    msg = Actor.receive
    msg.should == "test"

    Actor.unregister(:foo)
    Actor.lookup(:foo).should == nil
  end
end

describe "Actor.spawn_link" do
  it "sends an exit message to linked Actors" do
    actor = Actor.spawn_link do
      Actor.spawn_link do
        Actor.spawn_link do
          Actor.receive do |m|
            m.when(:die) { raise 'dying' }
          end
        end
      end
    end

    actor << :die
    msg = Actor.receive
    msg[0].should == :exit
  end
end

describe "Actor.link" do
  it "sends an exit message to linked Actors" do
    master = Actor.current
    chan = Channel.new

    a = Actor.spawn do
      msg = Actor.receive
      chan.send msg
    end

    b = Actor.spawn(Actor.current) do |master|
      Actor.link(master)
      Actor.link(a)

      raise "foo"
    end

    msgs = []
    msgs << Actor.receive
    msgs << chan.receive

    msgs[0][0].should == :exit
    msgs[1][0].should == :exit
  end
end

describe "Actor.unlink" do
  it "sends an exit message to linked Actors" do
    actor = Actor.spawn(Actor.current) do |master|
      Actor.link(master)

      Actor.receive do |f|
        f.when(:ping) { Actor.unlink(master) }
      end

      raise "foo"
    end

    actor.send :ping
    msg = Actor.receive do |f|
      f.after(0) do end
    end

    msg.should == nil
  end
end

