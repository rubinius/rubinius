require File.dirname(__FILE__) + '/../spec_helper'

describe "The retry statement" do
  it "raises a LocalJumpError if used outside of a block" do
    def bad_meth_retry; retry; end
    lambda { bad_meth_retry()      }.should raise_error(LocalJumpError)
    lambda { lambda { retry }.call }.should raise_error(LocalJumpError)
  end
  
  it "re-executes the closest block" do
    retry_first = true
    retry_second = true
    results = []
    begin
      results << 1
      raise
    rescue
      results << 2
      if retry_first
        results << 3
        retry_first = false
        retry
      end
      begin
        results << 4
        raise
      rescue
        results << 5
        if retry_second
          results << 6
          retry_second = false
          retry
        end
      end
    end

    results.should == [1, 2, 3, 1, 2, 4, 5, 6, 4, 5]
  end

  # block retry has been officially deprecated by matz and is unsupported in 1.9
  ruby_version_is "" ... "1.9" do
    not_compliant_on :rubinius do
      it "re-executes the entire enumeration" do
        list = []
        [1,2,3].each do |x|
          list << x
          break if list.size == 6
          retry if x == 3
        end
        list.should == [1,2,3,1,2,3]
      end
    end
  end
end

describe "The retry keyword inside a begin block's rescue block" do
  it "causes the begin block to be executed again" do
    counter = 0
    
    begin
      counter += 1
      raise "An exception"
    rescue
      retry unless counter == 7
    end
    
    counter.should == 7
  end
end
