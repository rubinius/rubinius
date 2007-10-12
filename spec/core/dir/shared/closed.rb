@dir_closed = shared "Dir closed" do |cmd|
  describe "Dir##{cmd}" do
    it "raises an IOError when called on a closed Dir instance" do
      should_raise(IOError) do
        dir = Dir.open mock_dir
        dir.close
        dir.send cmd
      end
    end
  end
end
