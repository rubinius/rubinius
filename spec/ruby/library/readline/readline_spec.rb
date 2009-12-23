require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do

  not_supported_on :ironruby, :jruby do
    require 'readline'

    describe "Readline.readline" do
      before :each do
        @file = tmp('readline')
        File.open(@file, 'w') do |file|
          file.puts "test\n"
        end
        @stdin_back = STDIN.dup
        @stdout_back = STDOUT.dup
        STDIN.reopen(@file, 'r')
        STDOUT.reopen("/dev/null")
      end

      after :each do
        rm_r @file
        STDIN.reopen(@stdin_back)
        STDOUT.reopen(@stdout_back)
      end

      it "returns the input string" do
        Readline.readline.should == "test"
      end

      it "taints the returned strings" do
        Readline.readline.tainted?.should be_true
      end
    end
  end
end
