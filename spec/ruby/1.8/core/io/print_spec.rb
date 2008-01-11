require File.dirname(__FILE__) + '/../../spec_helper'

describe IO, "#print" do
  before :each do
    @old_separator = $\
    $\ = '->'
  end

  after :each do
    $\ = @old_separator
  end

  # Fails due to incomplete output speccing
  it "writes $_.to_s followed by $\\ (if any) to the stream if no arguments given" do
    $_ = nil
    lambda { $stdout.print }.should output("#{$\}")

    string = File.open(__FILE__) {|f| f.gets }  # Set $_ to something known
    lambda { $stdout.print }.should output("#{string}#{$\}")
  end

  it "writes obj.to_s followed by $\\ (if any) to the stream when given one object" do
    o = Object.new
    def o.to_s(); 'I am an object'; end

    lambda { $stdout.print(o) }.should output("#{o.to_s}#{$\}")
  end

  it "does not call obj.to_str" do
    o = Object.new
    def o.to_str(); 'Haha!'; end
    def o.to_s(); 'I am an object'; end

    lambda { $stdout.print(o) }.should output("#{o.to_s}#{$\}")
  end

  it "writes each obj.to_s to the stream and appends $\\ (if any) given multiple objects" do
    o, o2 = Object.new, Object.new
    def o.to_s(); 'o'; end
    def o2.to_s(); 'o2'; end

    lambda { $stdout.print(o, o2) }.should output("#{o.to_s}#{o2.to_s}#{$\}")
  end
end
