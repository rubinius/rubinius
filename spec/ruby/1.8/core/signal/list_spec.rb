require File.dirname(__FILE__) + '/../../spec_helper'

describe "Signal.list" do
  RUBY18_SIGNALS = %w{
    EXIT
    HUP
    INT
    QUIT
    ILL
    TRAP
    IOT
    ABRT
    EMT
    FPE
    KILL
    BUS
    SEGV
    SYS
    PIPE
    ALRM
    TERM
    URG
    STOP
    TSTP
    CONT
    CHLD
    CLD
    TTIN
    TTOU
    IO
    XCPU
    XFSZ
    VTALRM
    PROF
    WINCH
    USR1
    USR2
    LOST
    MSG
    PWR
    POLL
    DANGER
    MIGRATE
    PRE
    GRANT
    RETRACT
    SOUND
    INFO
  }

  LINUX_SIGNALS = {"PWR"=>30, "USR1"=>10, "BUS"=>7, "USR2"=>12, "TERM"=>15, 
    "SEGV"=>11, "KILL"=>9, "EXIT"=>0, "POLL"=>29, "STOP"=>19, "SYS"=>31,
    "TRAP"=>5, "IOT"=>6, "HUP"=>1, "INT"=>2, "WINCH"=>28, "XCPU"=>24, 
    "TTIN"=>21, "CLD"=>17, "TSTP"=>20, "FPE"=>8, "IO"=>29, "TTOU"=>22, 
    "PROF"=>27, "CHLD"=>17, "CONT"=>18, "PIPE"=>13, "ABRT"=>6, "VTALRM"=>26, 
    "QUIT"=>3, "ILL"=>4, "XFSZ"=>25, "URG"=>23, "ALRM"=>14}

  DARWIN_SIGNALS = {"USR1"=>30, "BUS"=>10, "USR2"=>31, "TERM"=>15, "SEGV"=>11,
    "KILL"=>9, "EMT"=>7, "EXIT"=>0, "STOP"=>17, "SYS"=>12, "TRAP"=>5, 
    "INFO"=>29, "IOT"=>6, "HUP"=>1, "INT"=>2, "WINCH"=>28, "XCPU"=>24, 
    "TTIN"=>21, "CLD"=>20, "TSTP"=>18, "FPE"=>8, "IO"=>23, "TTOU"=>22, 
    "PROF"=>27, "CHLD"=>20, "CONT"=>19, "PIPE"=>13, "ABRT"=>6, "VTALRM"=>26, 
    "QUIT"=>3, "ILL"=>4, "XFSZ"=>25, "URG"=>16, "ALRM"=>14}

  it "returns a hash of signal names mapped to the corresponding signal numbers" do
    # FIXME: values may change trough the arches too
    platform_is :linux do
      Signal.list.should == LINUX_SIGNALS
    end
    platform_is :darwin do
      Signal.list.should == DARWIN_SIGNALS
    end
    platform_is_not :linux, :darwin do
      raise "FIXME, please add platform support"
    end
  end

  it "doesn't contain other signals than in 1.8" do
    (Signal.list.keys - RUBY18_SIGNALS).should == []
  end

  if Signal.list["CHLD"]
    it "should redefine CLD with CHLD if defined" do
      Signal.list["CLD"].should == Signal.list["CHLD"]
    end
  end

  it "should contain the EXIT key with a value of zero" do
    Signal.list["EXIT"].should == 0
  end
end
