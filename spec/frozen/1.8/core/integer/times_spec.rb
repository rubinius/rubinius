require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer#times with block argument" do
  it "passes block values from 0 to self - 1" do
    a = []
    9.times { |i| a << i }
    -2.times { |i| a << i }
    a.should == [0, 1, 2, 3, 4, 5, 6, 7, 8]
  end

  it "executes a simple expression" do
    a = 0
    b = 3.times do |i|
      a += 1
    end
    a.should == 3
    b.should == 3
  end

  it "skips the current iteration of the block from 'next' to the end" do
    a = 0
    b = 3.times do |i|
      next if a == 2
      a += 1
    end
    a.should == 2
    b.should == 3
  end

  it "stops executing the block when it contains a break expression" do
    a = 0
    b = 3.times do |i|
      a += 1
      break
    end
    a.should == 1
    b.should == nil
  end

  it "stops executing the block when it contains a break expression with argument" do
    a = 0
    b = 3.times do |i|
      a += 1
      break 9
    end
    a.should == 1
    b.should == 9
  end

  it "stops executing the block when it contains an expression containing break" do
    a = [false]
    b = 1.times do |i|
      a.shift or break
    end
    a.should == []
    b.should == nil
  end

  it "stops executing the block when it contains a complex break expression" do
    a = [false]
    b = 2.times do |i|
      (a.shift or break) << 'x'
    end
    a.should == []
    b.should == nil
  end

  it "executes a nested while loop containing a break expression" do
    a = [false]
    b = 1.times do |i|
      while true
        a.shift or break
      end
    end
    a.should == []
    b.should == 1
  end

  it "executes a nested while loop containing a complex break expression" do
    a = [nil, nil]
    b = 3.times do |i|
      while true
        (a.shift or break) << 'x'
      end
    end
    a.should == []
    b.should == 3
  end

  it "executes a nested #times" do
    a = 0
    b = 3.times do |i|
      2.times { a += 1 }
    end
    a.should == 6
    b.should == 3
  end
end

describe "Integer#times without block argument" do
  it "executes a simple expression" do
    a = 0
    b = 3.times do
      a += 1
    end
    a.should == 3
    b.should == 3
  end

  it "skips the current iteration of the block from 'next' to the end" do
    a = 0
    b = 3.times do
      next if a == 2
      a += 1
    end
    a.should == 2
    b.should == 3
  end

  it "stops executing the block when it contains a break expression" do
    a = 0
    b = 3.times do
      a += 1
      break
    end
    a.should == 1
    b.should == nil
  end

  it "stops executing the block when it contains a break expression with argument" do
    a = 0
    b = 3.times do
      a += 1
      break 9
    end
    a.should == 1
    b.should == 9
  end

  it "stops executing the block when it contains an expression containing break" do
    a = [false]
    b = 1.times do
      a.shift or break
    end
    a.should == []
    b.should == nil
  end

  it "stops executing the block when it contains a complex break expression" do
    a = [false]
    b = 2.times do
      (a.shift or break) << 'x'
    end
    a.should == []
    b.should == nil
  end

  it "executes a nested while loop containing a break expression" do
    a = [false]
    b = 1.times do
      while true
        a.shift or break
      end
    end
    a.should == []
    b.should == 1
  end

  it "executes a nested while loop containing a complex break expression" do
    a = [nil, nil]
    b = 3.times do
      while true
        (a.shift or break) << 'x'
      end
    end
    a.should == []
    b.should == 3
  end

  it "executes a nested #times" do
    a = 0
    b = 3.times do
      2.times { a += 1 }
    end
    a.should == 6
    b.should == 3
  end
end
