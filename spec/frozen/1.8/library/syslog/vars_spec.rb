require File.dirname(__FILE__) + '/../../spec_helper'

describe "Syslog::ident" do
  not_supported_on :windows do
    before :all do
      require 'syslog'
    end
  
    it 'should return the value of the last ident passed to open' do
      Syslog.open {|s| s.ident.should == $0 }
      Syslog.open("Rubinius") {|s| s.ident.should == "Rubinius" }
      Syslog.open("4") {|s| s.ident.should == "4" }
      Syslog.open("google") {|s| s.ident.should == "google" }
      Syslog.open("I should do my homework") {|s| s.ident.should == "I should do my homework" }
    end
  end
end

describe "Syslog::options" do
  not_supported_on :windows do
    before :all do
      require 'syslog'
    end
  
    it 'should default to (LOG_PID | LOG_CONS)' do
      Syslog.open {|s| s.options.should == (Syslog::LOG_PID | Syslog::LOG_CONS) }
    end
  
    it 'should return the value of the last "option" passed to open' do
      Syslog.open("Rubinius", 90) {|s| s.options.should == 90 }
      Syslog.open("Rubinius", 5) {|s| s.options.should == 5 }
    end
  end
end

describe "Syslog::facility" do
  not_supported_on :windows do
    before :all do
      require 'syslog'
    end
  
    it 'should default to LOG_USER' do
      Syslog.open {|s| s.facility.should == Syslog::LOG_USER }
    end
  
    it 'should return the value of the last "facility" passed to open' do
      Syslog.open("Rubinius", 5, 10) {|s| s.facility.should == 10 }
      Syslog.open("monkey", 99, 1) {|s| s.facility.should == 1 }
    end
  end
end

describe "Syslog::mask" do
  not_supported_on :windows do
    before :all do
      require 'syslog'
    end
  
    # TODO - This spec doesn't really check much. Figure out how to check the
    # real mask value set in the OS for this syslog
    it 'should default to 255' do
      Syslog.open { |s| s.mask.should == 255 }
    end
  end
end
