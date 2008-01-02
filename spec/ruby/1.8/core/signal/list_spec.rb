require File.dirname(__FILE__) + '/../../spec_helper'

describe "Signal.list" do
  it "returns a hash of signal names mapped to the corresponding signal numbers" do
    Signal.list.should == {"ABRT"=>6, "ALRM"=>14, "BUS"=>7, "CHLD"=>17,
      "CLD"=>17, "CONT"=>18, "FPE"=>8, "HUP"=>1, "ILL"=>4, "INT"=>2,
      "IO"=>29, "IOT"=>6, "KILL"=>9, "PIPE"=>13, "POLL"=>29, "PROF"=>27,
      "PWR"=>30, "QUIT"=>3, "SEGV"=>11, "STOP"=>19, "SYS"=>31,
      "TERM"=>15, "TRAP"=>5, "TSTP"=>20, "TTIN"=>21, "TTOU"=>22,
      "URG"=>23, "USR1"=>10, "USR2"=>12, "VTALRM"=>26, "WINCH"=>28,
      "XCPU"=>24, "XFSZ"=>25, "EXIT"=>0}
  end
end
