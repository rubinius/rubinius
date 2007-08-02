# mini_rspec.rb
#
# Very minimal set of features to support RSpec specs like this:
#
# describe "Array" do
#   it "responds to new" do
#     Array.new.should == []
#   end
# end

class SpecReporter
  class ExpectationReport
    def exception=(e)
      @exception = e
    end
    
    def exception
      @exception
    end
    
    def describe=(d)
      @describe = d
    end
    
    def describe
      @describe
    end
    
    def it=(i)
      @it = i
    end
    
    def it
      @it
    end
  end
  
  def initialize(out=STDOUT)
    @out = out
    @examples = 0
    @failures = 0
    @exceptions = []
  end
  
  attr_reader :describe
  
  def before_describe(msg)
    @describe = msg
  end
  
  def after_describe(msg); end
  
  def before_it(msg)
    @report = ExpectationReport.new
    @report.describe = @describe
    @report.it = msg
    @examples += 1
  end
  
  def after_it(msg); end
  
  def exception(e)
    @failures += 1
    @report.exception = e
    @exceptions.push(@report)
  end
  
  def summary
    unless @summarized
      @out.print "\n\n"
      @exceptions.each_with_index do |r,i|
        print_failure(i+1,r)
        print_backtrace(r.exception)
      end
      print_summary
      @summarized = true
    end
  end

  def print_summary
    @out.print @examples.to_s + " examples, " + @failures.to_s + " failures\n"
  end
  
  def print_failure(i,r)
    @out.print i.to_s + ") " + r.describe + " " + r.it + " FAILED\n"
  end
  
  def print_backtrace(e)
    if e.message != ""
      @out.print e.message + ": \n"

      begin
        @out.print e.backtrace.show
      rescue Exception
        @out.puts e.backtrace
      end

      @out.print "\n"
    else
      @out.print "<No message>"
    end
    @out.print "\n"
  end
end

class SpecDoxReporter < SpecReporter
  def before_describe(msg)
    super
    @out.print msg
    @out.print "\n-------------------\n"
  end
  
  def after_describe(msg)
    @out.print "\n"
  end
  
  def before_it(msg)
    super
    @out.print " - "
    @out.print msg
  end
  
  def after_it(msg)
    @out.print "\n"
  end
  
  def exception(e)
    super
    @out.print " FAILED"
  end
end

class DottedReporter < SpecReporter
  def after_it(msg)
    if @report.exception
      @out.print 'F'
    else
      @out.print '.'
    end
  end
end

class HtmlReporter < SpecReporter
end

class CIReporter < SpecReporter
  def print_backtrace(e)
  end
end

class ImmediateReporter < SpecReporter
  def after_it(msg)
    if @report.exception
      @out.print 'F'
      @out.print "\n" + @describe + " " + @report.it + " FAILED\n"
      print_backtrace(@report.exception)
    else
      @out.print '.'
    end
  end
  
  def summary
    unless @summarized
      @out.print "\n\n"
      print_summary
      @summarized = true
    end
  end
end

class PositiveExpectation
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    unless @obj == other
      raise Exception.new("Equality expected for #{@obj.inspect} and #{other.inspect}")
    end
  end
  
  def =~(other)
    unless @obj =~ other
      raise Exception.new("Match expected for #{@obj.inspect} and #{other.inspect}")
    end
  end
end

class NegativeExpectation
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    if @obj == other
      raise Exception.new("Inequality expected for #{@obj.inspect} and #{other.inspect}")
    end
  end
  
  def =~(other)
    if @obj =~ other
      raise Exception.new("Match not expected for #{@obj.inspect} and #{other.inspect}")
    end
  end
end

class Object
  def should
    PositiveExpectation.new(self)
  end
  
  def should_not
    NegativeExpectation.new(self)
  end
end

@__before__ = []
@__after__ = []

def before(at=:each,&block)
  if at == :each
    @__before__.push block
  elsif at == :all
    STDOUT.print "mini_rspec does not support before(:all)"
  else
    raise ArgumentError, "I do not know when you want me to call your block"
  end
end

def after(at=:each,&block)
  if at == :each
    @__after__.push block
  elsif at == :all
    STDOUT.print "mini_rspec does not support after(:all)"
  else
    raise ArgumentError, "I do not know when you want me to call your block"
  end
end

def it(msg)
  full_msg = @reporter.describe + " " + msg
  skip = false
  
  $spec_exclude_res.each do |re|
    skip = true if re.match(full_msg)
  end
  
  return if skip
  
  @reporter.before_it(msg)

  begin
    @__before__.each { |b| b.call }
    yield
    Mock.verify  

  rescue Exception => e
    @reporter.exception(e)

  # Cleanup
  ensure
    Mock.cleanup
    Mock.reset
    @__after__.each { |b| b.call }
  end
  
  @reporter.after_it(msg)
end

def describe(msg)
  @reporter.before_describe(msg)
  yield
  @reporter.after_describe(msg)
end

# Alternatives
class Object
  alias context describe
  alias specify it
  alias setup before
  alias teardown after
end

if @reporter == nil
  if rep = ENV['REPORTER']
    cls = Object.const_get(rep) rescue nil
    if cls.nil?
      puts "Unable to find reporter '#{rep}', falling back."
      @reporter = DottedReporter.new
    else
      @reporter = cls.new
    end
  else
    @reporter = DottedReporter.new
  end
end

at_exit do
  @reporter.summary
end
