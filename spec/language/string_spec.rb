# -*- encoding: us-ascii -*-
#
with_feature :encoding do
  describe "Ruby String interpolation" do
    it "creates a String when a string doesn't have an encoding yet" do
      a = "ascii" 
      b = "\x00+/\xa8"
      b.instance_variable_set("@encoding", nil)

      str = "#{a}:#{b}"
      str.should == "ascii:\x00+/\xa8"
      str.encoding.should == Encoding::ASCII_8BIT
    end
  end
end
