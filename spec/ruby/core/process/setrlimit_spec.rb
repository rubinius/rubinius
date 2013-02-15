require File.expand_path('../../../spec_helper', __FILE__)

describe "Process.setrlimit and Process.getrlimit" do
  platform_is_not :windows do
    it "coerces arguments to Integers" do
      lim, max = Process.getrlimit(Process::RLIMIT_CORE)
      Process.setrlimit(mock_int(Process::RLIMIT_CORE),
                        mock_int(lim),
                        mock_int(max)).should be_nil
    end

    it "limit and get core size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_CORE)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_CORE, lim, max).should be_nil
    end

    it "limit and get CPU time (seconds)" do
      lim, max = Process.getrlimit(Process::RLIMIT_CPU)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_CPU, lim, max).should be_nil
    end

    it "limit and get data segment (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_DATA)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_DATA, lim, max).should be_nil
    end

    it "limit and get file size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_FSIZE)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_FSIZE, lim, max).should be_nil
    end

    it "limit and get file descriptors (number)" do
      lim, max = Process.getrlimit(Process::RLIMIT_NOFILE)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_NOFILE, lim, max).should be_nil
    end

    it "limit and get stack size (bytes)" do
      lim, max = Process.getrlimit(Process::RLIMIT_STACK)
      lim.kind_of?(Integer).should == true
      max.kind_of?(Integer).should == true
      Process.setrlimit(Process::RLIMIT_STACK, lim, max).should be_nil
    end

    platform_is_not :openbsd do
      it "limit and get total available memory (bytes)" do
        lim, max = Process.getrlimit(Process::RLIMIT_AS)
        lim.kind_of?(Integer).should == true
        max.kind_of?(Integer).should == true
        Process.setrlimit(Process::RLIMIT_AS, lim, max).should be_nil
      end
    end

    platform_is_not :solaris do
      it "limit and get total size for mlock(2) (bytes)" do
        lim, max = Process.getrlimit(Process::RLIMIT_MEMLOCK)
        lim.kind_of?(Integer).should == true
        max.kind_of?(Integer).should == true
        max = lim if lim > max # EINVAL is raised if this invariant is violated
        Process.setrlimit(Process::RLIMIT_MEMLOCK, lim, max).should be_nil
      end

      it "limit and get number of processes for the user (number)" do
        lim, max = Process.getrlimit(Process::RLIMIT_NPROC)
        lim.kind_of?(Integer).should == true
        max.kind_of?(Integer).should == true
        Process.setrlimit(Process::RLIMIT_NPROC, lim, max).should be_nil
      end

      it "limit and get resident memory size (bytes)" do
        lim, max = Process.getrlimit(Process::RLIMIT_RSS)
        lim.kind_of?(Integer).should == true
        max.kind_of?(Integer).should == true
        Process.setrlimit(Process::RLIMIT_RSS, lim, max).should be_nil
      end
    end

    platform_is :os => [:netbsd, :freebsd] do
      it "limit and get all socket buffers (bytes)" do
        lim, max = Process.getrlimit(Process::RLIMIT_SBSIZE)
        lim.kind_of?(Integer).should == true
        max.kind_of?(Integer).should == true
        Process.setrlimit(Process::RLIMIT_SBSIZE , lim, max).should be_nil
      end
    end

    ruby_version_is "1.9" do
      describe "symbol to constant coerction" do
        platform_is_not :openbsd do
          it "coerces :AS into RLIMIT_AS" do
            Process.setrlimit(:AS, *Process.getrlimit(Process::RLIMIT_AS)).should be_nil
          end
        end

        it "coerces :CORE into RLIMIT_CORE" do
          Process.setrlimit(:CORE, *Process.getrlimit(Process::RLIMIT_CORE)).should be_nil
        end

        it "coerces :CPU into RLIMIT_CPU" do
          Process.setrlimit(:CPU, *Process.getrlimit(Process::RLIMIT_CPU)).should be_nil
        end

        it "coerces :DATA into RLIMIT_DATA" do
          Process.setrlimit(:DATA, *Process.getrlimit(Process::RLIMIT_DATA)).should be_nil
        end

        it "coerces :FSIZE into RLIMIT_FSIZE" do
          Process.setrlimit(:FSIZE, *Process.getrlimit(Process::RLIMIT_FSIZE)).should be_nil
        end

        it "coerces :NOFILE into RLIMIT_NOFILE" do
          Process.setrlimit(:NOFILE, *Process.getrlimit(Process::RLIMIT_NOFILE)).should be_nil
        end

        it "coerces :STACK into RLIMIT_STACK" do
          Process.setrlimit(:STACK, *Process.getrlimit(Process::RLIMIT_STACK)).should be_nil
        end

        platform_is_not :solaris do
          it "coerces :MEMLOCK into RLIMIT_MEMLOCK" do
            Process.setrlimit(:MEMLOCK, *Process.getrlimit(Process::RLIMIT_MEMLOCK)).should be_nil
          end

          it "coerces :NPROC into RLIMIT_NPROC" do
            Process.setrlimit(:NPROC, *Process.getrlimit(Process::RLIMIT_NPROC)).should be_nil
          end

          it "coerces :RSS into RLIMIT_RSS" do
            Process.setrlimit(:RSS, *Process.getrlimit(Process::RLIMIT_RSS)).should be_nil
          end
        end

        platform_is :os => [:netbsd, :freebsd] do
          it "coerces :SBSIZE into RLIMIT_SBSIZE" do
            Process.setrlimit(:SBSIZE, *Process.getrlimit(Process::RLIMIT_SBSIZE)).should be_nil
          end
        end

        describe "not present in Rubinius" do
          it "coerces :RTPRIO into RLIMIT_RTPRIO"
          it "coerces :RTTIME into RLIMIT_RTTIME"
          it "coerces :SIGPENDING into RLIMIT_SIGPENDING"
          it "coerces :MSGQUEUE into RLIMIT_MSGQUEUE"
          it "coerces :NICE into RLIMIT_NICE"
        end

        context "when passed an unknown resource" do
          it "raises ArgumentError" do
            lambda { Process.getrlimit(:FOO) }.should raise_error(ArgumentError)
          end
        end
      end
    end
  end
end
