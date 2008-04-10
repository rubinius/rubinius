require File.dirname(__FILE__) + '/../../spec_helper'
require 'mailbox'

describe "Mailbox#clear" do
  it "removes all messages" do
    @mbox = Mailbox.new
    (0..9).to_a.each {|v| @mbox.send v }
    @mbox.clear
    msg = @mbox.receive do |f|
      f.when(Object) do |msg| msg end
      f.after(0) do end
    end

    msg.should == nil
  end
end
