require File.dirname(__FILE__) + '/../spec_helper'

extension :rubinius do
  require 'options'

  class String
    def squish(); self.gsub(/\s+/, ''); end
  end

  extension :rubinius do

    describe 'Options.new' do
      it 'creates a blank set of options' do
        Options.new.class.should == Options
      end

      it 'optionally takes a block that yields the Options instance' do
        oid = nil

        opts = Options.new {|o| oid = o.object_id }

        opts.object_id.should == oid
        opts.class.should == Options
      end

      it 'may be given a message to be shown at the top of #usage' do
        o = Options.new 'My very own header'      
        o.usage.squish.should == 'Myveryownheader'
      end
    end

    describe 'Options#option' do
      before { @o = Options.new }

      it 'takes a string with three parts: short and long options and a description' do
        @o.option '-h --help Help'
      end

      it 'does not accept the definition string without leading dashes' do
        lambda { @o.option 'h help Help' }.should raise_error(ArgumentError)
      end

      it 'requires that all three parts are present' do
        lambda { @o.option '-h Help'  }.should raise_error(ArgumentError)
        lambda { @o.option '--h Help' }.should raise_error(ArgumentError)
      end

      it 'requires that the three parts are in order' do
        lambda { @o.option '--help -h Help' }.should raise_error(ArgumentError)
        lambda { @o.option '-h Help --help' }.should raise_error(ArgumentError)
      end

      it 'optionally takes a parameter to designate number of arguments wanted' do
        @o.option '-f --file File', :one
      end
    end

    shared :lib_options_usage_help do |cmd|
      describe 'Getting help from the option parser' do
        it "provides a ##{cmd} message constructed from the given header and options" do
          o = Options.new 'Some message here'
          o.option '-h --help Displays a help message'
          o.option '-f --foo Does nothing useful', :one

          s = "SomemessagehereOptions:-h--helpDisplaysahelpmessage-f--fooARGDoesnothinguseful"
          o.usage.squish.should == s
        end  
      end
    end

    describe 'Options#usage' do it_behaves_like :lib_options_usage_help, "usage" end
    describe 'Options#help' do it_behaves_like :lib_options_usage_help, "help" end
  
    describe 'Parse error handling' do
      it 'Raises an ArgumentError by default' do
        lambda { Options.new.parse '-h' }.should raise_error(ArgumentError)
      end  

      it 'Allows overriding of default by supplying a block to #on_error' do
        o = Options.new {|o| o.on_error { :error } }
        o.parse('-h').should == :error
      end  

      it 'Supplies the #on_error block with the Options object and the exception' do
        o = Options.new {|o| o.on_error {|op, ex| [op.class, ex.class] } }
        o.parse('-h').should == [Options, ArgumentError]
      end
    end  

    describe 'Parsing options using the configured parser' do
      before do 
        @o = Options.new 

        @o.option '-a --aa Aa'  
        @o.option '-b --bb Bb'  
        @o.option '-c --cc Cc'  
      end

      it 'raises an error if given empty input' do
        lambda { @o.parse '' }.should raise_error(ArgumentError)
        lambda { @o.parse [] }.should raise_error(ArgumentError)
      end

      it 'returns a Hash with given options as defined keys' do
        h = @o.parse '--aa'

        h.key?('aa').nil?.should == false
      end

      it 'makes given option available both as long and short version' do
        h = @o.parse '--aa'

        h.key?('a').nil?.should == false
        h.key?('aa').nil?.should == false
      end

      it 'sets the value of any given option without a parameter to true' do
        h = @o.parse '--aa'

        h.key?('a').should == true
        h.key?('aa').should == true
      end

      it 'places any given arguments in :args if they do not belong to options' do
        h = @o.parse '-a ARG'

        h[:args].should == ['ARG']
      end
    end

    describe 'Parsing short options without arguments' do
      before do 
        @o = Options.new 

        @o.option '-a --aa Aa'  
        @o.option '-b --bb Bb'  
        @o.option '-c --cc Cc'  
      end

      it 'takes short options separately' do
        h = @o.parse '-a -b -c'
    
        h.key?('a').should == true
        h.key?('aa').should == true
        h.key?('b').should == true
        h.key?('bb').should == true
        h.key?('c').should == true
        h.key?('cc').should == true
      end

      it 'takes short options combined' do
        h = @o.parse '-abc'
    
        h.key?('a').should == true
        h.key?('aa').should == true
        h.key?('b').should == true
        h.key?('bb').should == true
        h.key?('c').should == true
        h.key?('cc').should == true
      end

      it 'takes short options interspersed with nonoption-arguments' do
        h = @o.parse '-ab ARG -c'
    
        h.key?('a').should == true
        h.key?('aa').should == true
        h.key?('b').should == true
        h.key?('bb').should == true
        h.key?('c').should == true
        h.key?('cc').should == true
        h[:args].should == ['ARG']
      end
    end

    describe 'Parsing short options with arguments' do
      before do 
        @o = Options.new 

        @o.option '-n --none None'  
        @o.option '-o --one  One',  :one 
        @o.option '-m --many Many', :many
        @o.option '-a --any  Any',  :any
      end

      it 'defaults to :none specified arguments which means no following argument is captured' do 
        @o.parse('-n ARG')['n'].should == true
        @o.parse('-n ARG')['none'].should == true
      end

      it 'stores the argument(s) in an Array stored as the value of the option name' do
        @o.parse('-o ARG')['o'].should == ['ARG']
        @o.parse('-o ARG')['one'].should == ['ARG']
      end

      it 'accepts :one to denote a single argument' do
        @o.parse('-o ARG')['o'].should == ['ARG']
        @o.parse('-o ARG')['one'].should == ['ARG']
      end

      it 'ignores more than one argument when :one defined, extra go to :args' do
        h = @o.parse '-o ARG1 ARG2'

        h['o'].should == ['ARG1']
        h['one'].should == ['ARG1']
        h[:args].should == ['ARG2']
      end

      it 'accepts :many to indicate as many nonoption args as possible until next option' do
        h = @o.parse '-m ARG1 ARG2 ARG3 -n ARG4'

        h['m'].should == %w|ARG1 ARG2 ARG3| 
        h['many'].should == %w|ARG1 ARG2 ARG3| 
        h['n'].should == true
        h['none'].should == true
        h[:args].should == %w|ARG4| 
      end

      it 'accepts :any to indicate zero or as many as possible arguments' do
        @o.parse('-a -n')['a'].should == true
        @o.parse('-a -n')['any'].should == true
        @o.parse('-a ARG -n')['a'].should == ['ARG']
        @o.parse('-a ARG -n')['any'].should == ['ARG']
        @o.parse('-a ARG1 ARG2 ARG3 -n')['a'].should == %w|ARG1 ARG2 ARG3|
        @o.parse('-a ARG1 ARG2 ARG3 -n')['any'].should == %w|ARG1 ARG2 ARG3|
      end

      it 'fails if :one and no arguments given before next option' do
        lambda { @o.parse '-o'        }.should raise_error(ArgumentError)
        lambda { @o.parse '-o -n ARG' }.should raise_error(ArgumentError)
      end

      it 'fails if :many and one or no arguments given' do
        lambda { @o.parse '-m ARG'         }.should raise_error(ArgumentError)
        lambda { @o.parse '-m ARG -n ARG2' }.should raise_error(ArgumentError)
      end

      it 'assigns arguments only to the last in a set of combined short options ' do
        @o.parse('-nao ARG')['n'].should == true
        @o.parse('-nao ARG')['none'].should == true
        @o.parse('-nao ARG')['a'].should == true
        @o.parse('-nao ARG')['any'].should == true
        @o.parse('-nao ARG')['o'].should == ['ARG']
        @o.parse('-nao ARG')['one'].should == ['ARG']
      end
    end

    describe 'Parsing long options without arguments' do
      before do 
        @o = Options.new 

        @o.option '-a --aa Aa'  
        @o.option '-b --bb Bb'  
        @o.option '-c --cc Cc'  
      end

      it 'takes long options separately' do
        h = @o.parse '--aa --bb --cc'
    
        h.key?('a').should == true
        h.key?('aa').should == true
        h.key?('b').should == true
        h.key?('bb').should == true
        h.key?('c').should == true
        h.key?('cc').should == true
      end

      it 'takes long options interspersed with nonoption-arguments' do
        h = @o.parse '--aa ARG --cc'
    
        h.key?('a').should == true
        h.key?('aa').should == true
        h.key?('c').should == true
        h.key?('cc').should == true
        h[:args].should == ['ARG']
      end
    end

    describe 'Parsing long options with arguments' do
      before do 
        @o = Options.new 

        @o.option '-n --none None'  
        @o.option '-o --one  One',  :one 
        @o.option '-m --many Many', :many
        @o.option '-a --any  Any',  :any
      end

      it 'defaults to :none specified arguments which means no following argument is captured' do 
        @o.parse('--none ARG')['n'].should == true
        @o.parse('--none ARG')['none'].should == true
      end

      it 'stores the argument(s) in an Array stored as the value of the option name' do
        @o.parse('--one ARG')['o'].should == ['ARG']
        @o.parse('--one ARG')['one'].should == ['ARG']
      end

      it 'accepts :one to denote a single argument' do
        @o.parse('--one ARG')['o'].should == ['ARG']
        @o.parse('--one ARG')['one'].should == ['ARG']
      end

      it 'ignores more than one argument when :one defined' do
        h = @o.parse '--one ARG1 ARG2'

        h['o'].should == ['ARG1']
        h['one'].should == ['ARG1']
        h[:args].should == ['ARG2']
      end

      it 'accepts :many to indicate as many nonoption args as follow before the following option' do
        h = @o.parse '--many ARG1 ARG2 ARG3 -n ARG4'

        h['m'].should == %w|ARG1 ARG2 ARG3| 
        h['many'].should == %w|ARG1 ARG2 ARG3| 
        h['n'].should == true
        h['n'].should == true
        h[:args].should == %w|ARG4| 
      end

      it 'accepts :any to indicate zero or as many as possible arguments' do
        @o.parse('--any -n')['a'].should == true
        @o.parse('--any -n')['any'].should == true
        @o.parse('--any ARG -n')['a'].should == ['ARG']
        @o.parse('--any ARG -n')['any'].should == ['ARG']
        @o.parse('--any ARG1 ARG2 ARG3 -n')['a'].should == %w|ARG1 ARG2 ARG3|
        @o.parse('--any ARG1 ARG2 ARG3 -n')['any'].should == %w|ARG1 ARG2 ARG3|
      end

      it 'fails if :one and no arguments given' do
        lambda { @o.parse '--one'        }.should raise_error(ArgumentError)
        lambda { @o.parse '--one -n ARG' }.should raise_error(ArgumentError)
      end

      it 'fails if :many and one or no arguments given' do
        lambda { @o.parse '--many ARG'         }.should raise_error(ArgumentError)
        lambda { @o.parse '--many ARG -n ARG2' }.should raise_error(ArgumentError)
      end
    end

  end 
end
