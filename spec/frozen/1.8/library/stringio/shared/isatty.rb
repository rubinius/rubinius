shared :stringio_isatty do |cmd|
  describe "StringIO##{cmd}" do
    it "returns false" do
      StringIO.new('tty').send(cmd).should be_false
    end
  end
end
