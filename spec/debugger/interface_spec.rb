require File.expand_path('../spec_helper', __FILE__)
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
  
  it "matches step in commands (s[tep] [to] [n]) to the StepIn Command subclass" do
    # Step in
    @ifc.process_command(nil, "s")
    @cmd_map[Debugger::StepIn].executes.should == 1
    @ifc.process_command(nil, "step")
    @cmd_map[Debugger::StepIn].executes.should == 2
    @ifc.process_command(nil, "s to 12")
    @cmd_map[Debugger::StepIn].executes.should == 3
    @ifc.process_command(nil, "s 5")
    @cmd_map[Debugger::StepIn].executes.should == 4
    @ifc.process_command(nil, "step 2")
    @cmd_map[Debugger::StepIn].executes.should == 5
  end
    
  it "matches step in commands (n[ext] [to] [n]) to the StepNext Command subclass" do
    # Step next
    @ifc.process_command(nil, "n")
    @cmd_map[Debugger::StepNext].executes.should == 1
    @ifc.process_command(nil, "next")
    @cmd_map[Debugger::StepNext].executes.should == 2
    @ifc.process_command(nil, "n to 12")
    @cmd_map[Debugger::StepNext].executes.should == 3
    @ifc.process_command(nil, "n 5")
    @cmd_map[Debugger::StepNext].executes.should == 4
    @ifc.process_command(nil, "next 2")
    @cmd_map[Debugger::StepNext].executes.should == 5
  end
    
  it "matches step out commands (o[ut] [n]) to the StepOut Command subclass" do
    # Step out
    @ifc.process_command(nil, "o")
    @cmd_map[Debugger::StepOut].executes.should == 1
    @ifc.process_command(nil, "out")
    @cmd_map[Debugger::StepOut].executes.should == 2
    @ifc.process_command(nil, "o 1")
    @cmd_map[Debugger::StepOut].executes.should == 3
    @ifc.process_command(nil, "out 5")
    @cmd_map[Debugger::StepOut].executes.should == 4
  end
    
  it "matches quit commands (q[uit]) to the Quit Command subclass" do
    # Quit
    @ifc.process_command(nil, "q")
    @cmd_map[Debugger::Quit].executes.should == 1
    @ifc.process_command(nil, "quit")
    @cmd_map[Debugger::Quit].executes.should == 2
  end
  
  it "matches list source commands (l[ist] [<method>] [start [end]]) to the ListSource Command subclass" do
    # List source
    @ifc.process_command(nil, "l")
    @cmd_map[Debugger::ListSource].executes.should == 1
    @ifc.process_command(nil, "list")
    @cmd_map[Debugger::ListSource].executes.should == 2
    @ifc.process_command(nil, "l 1 17")
    @cmd_map[Debugger::ListSource].executes.should == 3
    @ifc.process_command(nil, "list 12-16")
    @cmd_map[Debugger::ListSource].executes.should == 4
    @ifc.process_command(nil, "l Float#==")
    @cmd_map[Debugger::ListSource].executes.should == 5
    @ifc.process_command(nil, "l Float#== 1 10")
    @cmd_map[Debugger::ListSource].executes.should == 6
  end
  
  it "matches list sexp commands (sexp [<method>]) to the ListSexp Command subclass" do
    # List sexp
    @ifc.process_command(nil, "sexp")
    @cmd_map[Debugger::ListSexp].executes.should == 1
    @ifc.process_command(nil, "sexp Object#to_s")
    @cmd_map[Debugger::ListSexp].executes.should == 2
  end
  
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
    
  it "matches show ivars commands (iv[ars]) to the ShowIVars Command subclass" do
    # Instance variables
    @ifc.process_command(nil, "iv")
    @cmd_map[Debugger::ShowIVars].executes.should == 1
    @ifc.process_command(nil, "ivars")
    @cmd_map[Debugger::ShowIVars].executes.should == 2
  end
  
  it "matches show cvars commands (cv[ars] [<class>]) to the ShowCVars Command subclass" do
    # Instance variables
    @ifc.process_command(nil, "cv")
    @cmd_map[Debugger::ShowCVars].executes.should == 1
    @ifc.process_command(nil, "cvars")
    @cmd_map[Debugger::ShowCVars].executes.should == 2
    @ifc.process_command(nil, "cvars File")
    @cmd_map[Debugger::ShowCVars].executes.should == 3
    @ifc.process_command(nil, "cv Debugger::Command")
    @cmd_map[Debugger::ShowCVars].executes.should == 4    
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
  
  it "matches any other command string to the Eval Command subclass" do
    # Eval
    @ifc.process_command(nil, "a = 12")
    @cmd_map[Debugger::EvalExpression].executes.should == 1
    @ifc.process_command(nil, "puts b")
    @cmd_map[Debugger::EvalExpression].executes.should == 2
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

  it "matches step in instruction commands (s[tep]i [to] [n]) to the StepInInstruction Command subclass" do
    # Step in instruction
    @ifc.process_command(nil, "si")
    @cmd_map[Debugger::StepInInstruction].executes.should == 1
    @ifc.process_command(nil, "stepi")
    @cmd_map[Debugger::StepInInstruction].executes.should == 2
    @ifc.process_command(nil, "si to 12")
    @cmd_map[Debugger::StepInInstruction].executes.should == 3
    @ifc.process_command(nil, "si 5")
    @cmd_map[Debugger::StepInInstruction].executes.should == 4
    @ifc.process_command(nil, "stepi 2")
    @cmd_map[Debugger::StepInInstruction].executes.should == 5
  end
    
  it "matches step in commands (n[ext]i [to] [n]) to the StepNextInstruction Command subclass" do
    # Step next instruction
    @ifc.process_command(nil, "ni")
    @cmd_map[Debugger::StepNextInstruction].executes.should == 1
    @ifc.process_command(nil, "nexti")
    @cmd_map[Debugger::StepNextInstruction].executes.should == 2
    @ifc.process_command(nil, "ni to 12")
    @cmd_map[Debugger::StepNextInstruction].executes.should == 3
    @ifc.process_command(nil, "ni 5")
    @cmd_map[Debugger::StepNextInstruction].executes.should == 4
    @ifc.process_command(nil, "nexti 2")
    @cmd_map[Debugger::StepNextInstruction].executes.should == 5
  end
    
  it "matches decode commands (d[ecode] [<method>] [start [end]]) to the ListBytecode Command subclass" do
    @ifc.process_command(nil, "d")
    @cmd_map[Debugger::ListBytecode].executes.should == 1
    @ifc.process_command(nil, "decode")
    @cmd_map[Debugger::ListBytecode].executes.should == 2
    @ifc.process_command(nil, "d String#len")
    @cmd_map[Debugger::ListBytecode].executes.should == 3
    @ifc.process_command(nil, "decode 1 15")
    @cmd_map[Debugger::ListBytecode].executes.should == 4
    @ifc.process_command(nil, "decode String#split 7-15")
    @cmd_map[Debugger::ListBytecode].executes.should == 5
  end
  
  it "matches show stack commands () to the ShowStack Command subclass" do
    @ifc.process_command(nil, "vs")
    @cmd_map[Debugger::ShowStack].executes.should == 1
    @ifc.process_command(nil, "vm stack")
    @cmd_map[Debugger::ShowStack].executes.should == 2
    @ifc.process_command(nil, "vs 5")
    @cmd_map[Debugger::ShowStack].executes.should == 3
    @ifc.process_command(nil, "vm stack 5 10")
    @cmd_map[Debugger::ShowStack].executes.should == 4
  end
  
  it "matches show sendsites commands () to the ShowSendSites Command subclass" do
    @ifc.process_command(nil, "vss")
    @cmd_map[Debugger::ShowSendSites].executes.should == 1
    @ifc.process_command(nil, "vm send sites")
    @cmd_map[Debugger::ShowSendSites].executes.should == 2
    @ifc.process_command(nil, "vss match")
    @cmd_map[Debugger::ShowSendSites].executes.should == 3
    @ifc.process_command(nil, "vm send sites split")
    @cmd_map[Debugger::ShowSendSites].executes.should == 4
  end
end
