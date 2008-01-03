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
    values.should == [6, 14, 10, 20, 20, 19, 7, 0, 8, 1, 4, 29, 2, 23, 6, 9, 13, 27,
      3, 11, 17, 12, 15, 5, 18, 21, 22, 16, 30, 31, 26, 28, 24, 25]
  end
end
