require File.dirname(__FILE__) + '/../../spec_helper'

describe "Syslog::ident" do
  before :all do
    require 'syslog'
  end

  it 'should return the value of the last ident passed to open' do
    Syslog.open {|s| s.ident.should == nil }
    Syslog.open("Rubinius") {|s| s.ident.should == "Rubinius" }
    Syslog.open("4") {|s| s.ident.should == "4" }
    Syslog.open("google") {|s| s.ident.should == "google" }
    Syslog.open("I should do my homework") {|s| s.ident.should == "I should do my homework" }
  end
end

describe "Syslog::options" do
  before :all do
    require 'syslog'
  end

  it 'should return the value of the last "option" passed to open' do
    Syslog.open {|s| s.options.should == nil }
    Syslog.open("Rubinius", 90) {|s| s.options.should == 90 }
    Syslog.open("Rubinius", 5) {|s| s.options.should == 5 }
  end
end

describe "Syslog::facility" do
  before :all do
    require 'syslog'
  end

  it 'should return the value of the last "facility" passed to open' do
    Syslog.open {|s| s.facility.should == nil }
    Syslog.open("Rubinius", 5, 10) {|s| s.facility.should == 10 }
    Syslog.open("monkey", 99, 1) {|s| s.facility.should == 1 }
  end
end
