require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor.link" do
  it "sends an exit message to linked Actors" do
    master = Actor.current
    chan = Rubinius::Channel.new

    a = Actor.spawn do
      Actor.trap_exit = true
      msg = Actor.receive
      chan.send msg
    end

    b = Actor.spawn(Actor.current) do |master|
      Actor.trap_exit = true
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
