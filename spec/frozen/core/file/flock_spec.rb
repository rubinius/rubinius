require File.dirname(__FILE__) + '/../../spec_helper'

describe "File#flock" do
  before :each do
    system "echo 'rubinius' > flock_test"
  end

  after :each do
    File.delete('flock_test') if File.exist?('flock_test')
  end

  it "should lock a file" do
    f = File.open('flock_test', "r")
    f.flock(File::LOCK_EX).should == 0
    File.open('flock_test', "w") do |f2|
      f2.flock(File::LOCK_EX | File::LOCK_NB).should == false
    end
    f.flock(File::LOCK_UN).should == 0
    File.open('flock_test', "w") do |f2|
      f2.flock(File::LOCK_EX | File::LOCK_NB).should == 0
      f2.flock(File::LOCK_UN).should == 0
    end
    f.close
  end
end
