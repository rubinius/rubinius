require File.dirname(__FILE__) + '/../../spec_helper'

describe "Syslog::open" do
  not_supported_on :windows do
    before :all do
      require 'syslog'
    end
  
    it 'should return Syslog' do
      Syslog.open.should == Syslog
      Syslog.close    # Closing the log
      Syslog.open("Rubinius").should == Syslog
      Syslog.close    # Closing the log
      Syslog.open("Rubinius", 5, 9).should == Syslog
      Syslog.close
    end
    
    it 'should be able to take a block' do
      Syslog.open {|s| s.should == Syslog }
      Syslog.open("Rubinius") {|s| s.should == Syslog }
      Syslog.open("r", 5, 10) {|s| s.should == Syslog }
      lambda { Syslog.open; Syslog.close }.should_not raise_error
    end
    
    it 'should raise an error if the log is opened' do
      Syslog.open
      lambda { Syslog.open }.should raise_error
      lambda { Syslog.close; Syslog.open }.should_not raise_error
      Syslog.close    # So we can do more tests!
    end
  end
end