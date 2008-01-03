require File.dirname(__FILE__) + '/../../spec_helper'

describe "Signal.list" do
  it "returns a hash of signal names mapped to the corresponding signal numbers" do
    list = Signal.list
    keys = list.keys.sort
    values = keys.map {|key| list[key]}

    keys.should == ["ABRT", "ALRM", "BUS", "CHLD", "CLD", "CONT", "EMT", "EXIT",
      "FPE", "HUP", "ILL", "INFO", "INT", "IO", "IOT", "KILL", "PIPE", "PROF",
      "QUIT", "SEGV", "STOP", "SYS", "TERM", "TRAP", "TSTP", "TTIN", "TTOU", "URG",
      "USR1", "USR2", "VTALRM", "WINCH", "XCPU", "XFSZ"]
    values.should == [7, 15, 11, 21, 22, 20, 8, 0, 9, 1, 4, 31, 2, 25, 6, 10, 14,
      29, 3, 12, 18, 13, 16, 5, 19, 23, 24, 17, 32, 33, 28, 30, 26, 27]
  end
end
