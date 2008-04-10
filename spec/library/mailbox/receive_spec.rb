require File.dirname(__FILE__) + '/../../spec_helper'
require 'mailbox'

describe "Mailbox#receive" do
  before :each do
    @mbox = Mailbox.new
  end

  it "returns objects" do
    objects = ["hello there", 100, :asdf, Object.new]

    objects.each {|o| @mbox.send(o) }

    results = []
    objects.size.times { results << @mbox.receive }
    results.should == objects
  end

  it "filters on message type" do
    Foo = Struct.new :item
    Bar = Struct.new :item
    Baz = Struct.new :item
    Bla = Struct.new :item

    @mbox.send(Foo.new("aaaa"))
    @mbox.send(Bar.new("bbbb"))
    @mbox.send(Baz.new(100))
    @mbox.send(Bla.new(:asdf))

    @mbox.receive do |f|
      f.when Baz do |msg| msg.item end
    end.should == 100

    @mbox.receive do |f|
      f.when Foo do |msg| msg.item end
    end.should == "aaaa"
  end

  it "times out using filter#after with [seconds]" do
    complete = 0
    @mbox.receive do |f|
      f.when Object do |value| value end
      f.after(0.001) do
        complete += 1
      end
    end
    complete.should == 1

    items = [:foo, "bar", 100]
    items.each {|v| @mbox.send v }
    (items.size + 10).times do
      @mbox.receive do |f|
        f.when Object do |value| value end
        f.after(0) do
          complete += 1
        end
      end
    end
    complete.should == 11
  end
end
