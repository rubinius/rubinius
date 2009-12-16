require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.setrlimit" do
  platform_is_not :windows do
    it "requires at least two arguments" do
      lambda {
        Process.setrlimit(Process::RLIMIT_CORE)
      }.should raise_error(ArgumentError)
    end
  end
end

describe "Process.getrlimit" do
  it "requires one argument" do
    lambda { Process.getrlimit }.should raise_error(ArgumentError)
  end
end

describe "Process.setrlimit and Process.getrlimit" do
  platform_is_not :windows do
    it "limit and get core size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_CORE)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_CORE, lim, max).should == nil
    end

    it "limit and get CPU time (seconds)" do
      lim, max = Process.getrlimit(Process::RLIMIT_CPU)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_CPU, lim, max).should == nil
    end

    it "limit and get data segment (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_DATA)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_DATA, lim, max).should == nil
    end

    it "limit and get file size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_FSIZE)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_FSIZE, lim, max).should == nil
    end

    it "limit and get file descriptors (number)" do
      lim, max = Process.getrlimit(Process::RLIMIT_NOFILE)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_NOFILE, lim, max).should == nil
    end

    it "limit and get stack size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_STACK)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_STACK, lim, max).should == nil
    end

    platform_is_not :openbsd do
      it "limit and get total available memory (bytes)" do
        lim, max = Process.getrlimit(Process::RLIMIT_AS)
        lim.kind_of?(Integer).should == true
        max.kind_of?(Integer).should == true
        Process.setrlimit(Process::RLIMIT_AS, lim, max).should == nil
      end
    end

    it "limit and get total size for mlock(2) (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_MEMLOCK)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      max = lim if lim > max # EINVAL is raised if this invariant is violated
      Process.setrlimit(Process::RLIMIT_MEMLOCK, lim, max).should == nil
    end

    it "limit and get number of processes for the user (number)" do
      lim, max = Process.getrlimit(Process::RLIMIT_NPROC)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_NPROC, lim, max).should == nil
    end

    it "limit and get resident memory size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_RSS)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_RSS, lim, max).should == nil
    end

    platform_is :os => [:netbsd, :freebsd] do
      it "limit and get all socket buffers (bytes)" do
        #TODO
        # lim = Process.setrlimit(Process::RLIMIT_SBSIZE, )
        # lim.kind_of?(Integer).should == true
        # max.kind_of?(Integer).should == true
        # Process.setrlimit(Process::RLIMIT_SBSIZE , ).should == nil
      end
    end
  end
end
