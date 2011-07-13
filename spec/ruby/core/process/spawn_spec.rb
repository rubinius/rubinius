require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Process.spawn" do
    it "runs the specified command" do
      begin
        pid = Process.spawn(
            "#{RUBY_EXE} -e 'File.open(\".spawn.tmp\", \"w\") {|f| f.write(\"hello\")}'")
        Process.waitpid(pid)
        File.read('.spawn.tmp').should == "hello"
      ensure
        File.delete('.spawn.tmp')
      end
    end
    
    it "passes environment vars to the child environment" do
      begin
        pid = Process.spawn(
            {"FOO" => "BAR"},
            "#{RUBY_EXE} -e 'File.open(\".spawn.tmp\", \"w\") {|f| f.write(ENV[\"FOO\"])}'")
        Process.waitpid(pid)
        File.read('.spawn.tmp').should == "BAR"
      ensure
        File.delete('.spawn.tmp')
      end
    end
    
    it "needs to be reviewed for spec completeness"
  end
end
