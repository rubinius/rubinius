require File.dirname(__FILE__) + "/spec_helper"
require 'debugger/interface'
require 'stringio'

describe "Debugger::CmdLineInterface#handle_exception" do
  before :each do
    @out, @err = StringIO.new, StringIO.new
    @cl = Debugger::CmdLineInterface.new(@out, @err)
    @dbg = mock 'Debugger'
  end
  
  it "omits stack frames above Debugger::Interface#process_command" do
    @cl.eval_context = MethodContext.current
    @cl.process_command @dbg, 'blah'
    @err.string.should_not =~ /process_command/
  end
end

describe "Interface#process_command" do
  before do
    @ifc = Debugger::Interface.new
    @ifc.load_commands
    @cmd_map = {}
    @ifc.commands.each do |cmd|
      @cmd_map[cmd.class] = cmd

      def cmd.execute(dbg, interface, md)
        @executes = (@executes || 0) + 1
      end

      def cmd.executes
        @executes
      end
    end
  end

  it "matches help commands (?, h[elp]) to the Help Command subclass" do
    # Help
    @ifc.process_command(nil, "h")
    @cmd_map[Debugger::Help].executes.should == 1
    @ifc.process_command(nil, "help")
    @cmd_map[Debugger::Help].executes.should == 2
    @ifc.process_command(nil, "?")
    @cmd_map[Debugger::Help].executes.should == 3
  end
  
  it "matches list breakpoint commands (b[reak]) to the ShowBreakpoint Command subclass" do
    # List breakpoints
    @ifc.process_command(nil, "b")
    @cmd_map[Debugger::ShowBreakpoints].executes.should == 1
    @ifc.process_command(nil, "break")
    @cmd_map[Debugger::ShowBreakpoints].executes.should == 2
  end
  
  it "matches delete breakpoint commands (b[reak] d[el] <n>) to the DeleteBreakpoint Command subclass" do
    # Delete breakpoint
    @ifc.process_command(nil, "b d 1")
    @cmd_map[Debugger::DeleteBreakpoint].executes.should == 1
    @ifc.process_command(nil, "break del 55")
    @cmd_map[Debugger::DeleteBreakpoint].executes.should == 2
  end
  
  it "matches enable/disable breakpoint commands (b[reak] en|dis[able] <n>) to the EnableBreakpoint Command subclass" do
    # Enable/disable breakpoints
    @ifc.process_command(nil, "b en 1")
    @cmd_map[Debugger::EnableBreakpoint].executes.should == 1
    @ifc.process_command(nil, "break disable 5")
    @cmd_map[Debugger::EnableBreakpoint].executes.should == 2
  end  
  
  it "matches continue commands (c[ontinue], r[un]) to the Continue Command subclass" do
    # Continue
    @ifc.process_command(nil, "c")
    @cmd_map[Debugger::Continue].executes.should == 1
    @ifc.process_command(nil, "continue")
    @cmd_map[Debugger::Continue].executes.should == 2
    @ifc.process_command(nil, "r")
    @cmd_map[Debugger::Continue].executes.should == 3
    @ifc.process_command(nil, "run")
    @cmd_map[Debugger::Continue].executes.should == 4
  end
  
    # Step in
    
    # Step next
    
    # Step out
    
  it "matches quit commands (q[uit]) to the Quit Command subclass" do
    # Quit
    @ifc.process_command(nil, "q")
    @cmd_map[Debugger::Quit].executes.should == 1
    @ifc.process_command(nil, "quit")
    @cmd_map[Debugger::Quit].executes.should == 2
  end
  
    # List source
    
    # List sexp
  
  it "matches show locals commands ([l]v[ars]) to the ShowLocals Command subclass" do
    # Local variables
    @ifc.process_command(nil, "v")
    @cmd_map[Debugger::ShowLocals].executes.should == 1
    @ifc.process_command(nil, "lv")
    @cmd_map[Debugger::ShowLocals].executes.should == 2
    @ifc.process_command(nil, "lvars")
    @cmd_map[Debugger::ShowLocals].executes.should == 3
  end
  
  it "matches show globals commands (gv[ars]) to the ShowGlobals Command subclass" do
    # Global variables
    @ifc.process_command(nil, "gv")
    @cmd_map[Debugger::ShowGlobals].executes.should == 1
    @ifc.process_command(nil, "gvars")
    @cmd_map[Debugger::ShowGlobals].executes.should == 2
  end
    
  it "matches show ivars commands (iv[ars]) to the ShowIvars Command subclass" do
    # Instance variables
    @ifc.process_command(nil, "iv")
    @cmd_map[Debugger::ShowIVars].executes.should == 1
    @ifc.process_command(nil, "ivars")
    @cmd_map[Debugger::ShowIVars].executes.should == 2
  end
  
  it "matches backtrace commands (b[ack]t[race], w[here]) to the ShowBacktrace Command subclass" do
    # Backtrace
    @ifc.process_command(nil, "bt")
    @cmd_map[Debugger::ShowBacktrace].executes.should == 1
    @ifc.process_command(nil, "backtrace")
    @cmd_map[Debugger::ShowBacktrace].executes.should == 2
    @ifc.process_command(nil, "w")
    @cmd_map[Debugger::ShowBacktrace].executes.should == 3
    @ifc.process_command(nil, "where")
    @cmd_map[Debugger::ShowBacktrace].executes.should == 4
  end  
  
  it "matches up frame commands (up <n>) to the UpFrame Command subclass" do
    # Up frame
    @ifc.process_command(nil, "up")
    @cmd_map[Debugger::UpFrame].executes.should == 1
    @ifc.process_command(nil, "up 3")
    @cmd_map[Debugger::UpFrame].executes.should == 2
  end
    
  it "matches down frame commands (down <n>) to the DownFrame Command subclass" do
    # Down frame
    @ifc.process_command(nil, "down")
    @cmd_map[Debugger::DownFrame].executes.should == 1
    @ifc.process_command(nil, "down 2")
    @cmd_map[Debugger::DownFrame].executes.should == 2
  end
end
