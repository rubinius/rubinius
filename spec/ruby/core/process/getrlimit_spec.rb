require File.expand_path('../../../spec_helper', __FILE__)

# see setrlimit_spec.rb

describe "Process.getrlimit" do
  it "needs to be reviewed for spec completeness"

  ruby_version_is "1.9" do
    platform_is_not :windows do
      describe "symbol to constant cooercion" do

        platform_is_not :openbsd do
          it "coerces :AS into RLIMIT_AS" do
            Process.getrlimit(:AS).should == Process.getrlimit(Process::RLIMIT_AS)
          end
        end

        it "coerces :CORE into RLIMIT_CORE" do
          Process.getrlimit(:CORE).should == Process.getrlimit(Process::RLIMIT_CORE)
        end

        it "coerces :CPU into RLIMIT_CPU" do
          Process.getrlimit(:CPU).should == Process.getrlimit(Process::RLIMIT_CPU)
        end

        it "coerces :DATA into RLIMIT_DATA" do
          Process.getrlimit(:DATA).should == Process.getrlimit(Process::RLIMIT_DATA)
        end

        it "coerces :FSIZE into RLIMIT_FSIZE" do
          Process.getrlimit(:FSIZE).should == Process.getrlimit(Process::RLIMIT_FSIZE)
        end

        it "coerces :NOFILE into RLIMIT_NOFILE" do
          Process.getrlimit(:NOFILE).should == Process.getrlimit(Process::RLIMIT_NOFILE)
        end

        it "coerces :STACK into RLIMIT_STACK" do
          Process.getrlimit(:STACK).should == Process.getrlimit(Process::RLIMIT_STACK)
        end

        platform_is_not :solaris do
          it "coerces :MEMLOCK into RLIMIT_MEMLOCK" do
            Process.getrlimit(:MEMLOCK).should == Process.getrlimit(Process::RLIMIT_MEMLOCK)
          end

          it "coerces :NPROC into RLIMIT_NPROC" do
            Process.getrlimit(:NPROC).should == Process.getrlimit(Process::RLIMIT_NPROC)
          end

          it "coerces :RSS into RLIMIT_RSS" do
            Process.getrlimit(:RSS).should == Process.getrlimit(Process::RLIMIT_RSS)
          end
        end

        platform_is :os => [:netbsd, :freebsd] do
          it "coerces :SBSIZE into RLIMIT_SBSIZE" do
            Process.getrlimit(:SBSIZE).should == Process.getrlimit(Process::RLIMIT_SBSIZE)
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
