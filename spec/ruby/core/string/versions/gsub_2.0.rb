describe :string_gsub_named_capture, :shared => true do
  it "replaces \\k named backreferences with the regexp's corresponding capture" do
    str = "hello"

    str.gsub(/(?<foo>[aeiou])/, '<\k<foo>>').should == "h<e>ll<o>"
    str.gsub(/(?<foo>.)/, '\k<foo>\k<foo>').should == "hheelllloo"
  end
end
