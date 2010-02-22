require File.expand_path('../../../spec_helper', __FILE__)

describe "File#flock" do
  before :each do
    @name = tmp("flock_test")
    touch(@name) { |f| f.write "rubinius" }

    @file = File.open @name, "r"
  end

  after :each do
    @file.flock File::LOCK_UN
    @file.close
    rm_r @name
  end

  it "exclusively locks a file" do
    @file.flock(File::LOCK_EX).should == 0
    @file.flock(File::LOCK_UN).should == 0
  end

  it "non-exclusively locks a file" do
    @file.flock(File::LOCK_SH).should == 0
    @file.flock(File::LOCK_UN).should == 0
  end

  it "returns false if trying to lock an exclusively locked file" do
    @file.flock File::LOCK_EX

    File.open(@name, "w") do |f2|
      f2.flock(File::LOCK_EX | File::LOCK_NB).should == false
    end
  end

  it "returns 0 if trying to lock a non-exclusively locked file" do
    @file.flock File::LOCK_SH

    File.open(@name, "w") do |f2|
      f2.flock(File::LOCK_SH | File::LOCK_NB).should == 0
      f2.flock(File::LOCK_UN).should == 0
    end
  end
end
