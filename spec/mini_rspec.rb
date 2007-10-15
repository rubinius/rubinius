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
      @describe = d.to_s
    end
    
    def describe
      @describe
    end
    
    def it=(i)
      @it = i.to_s
    end
    
    def it
      @it
    end
  end
  
  def initialize(out=STDOUT)
    self.out = out
    @examples = 0
    @failures = 0
    @exceptions = []
  end
  
  def out=(out)
    if out.is_a?(IO)
      @out = out
    else
      @out = File.open(out, "w")
    end
  end
  
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
  
  def summarized=(flag)
    @summarized = flag
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
    @out.print i.to_s + ")\n" + r.describe + " " + r.it + " FAILED\n"
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
    @describe_printed = false
  end
  
  def print_describe
    unless @describe_printed
      @out.print "\n" + @describe.to_s + "\n"
      @describe_printed = true
    end
  end
  
  def before_it(msg)
    super
    print_describe
    @out.print "- " + msg
  end
  
  def after_it(msg)
    @out.print "\n"
  end
  
  def exception(e)
    super
    if e.is_a?(ExpectationNotMetError)
      @out.print " (FAILED - " + @failures.to_s + ")"
    else
      @out.print " (ERROR - " + @failures.to_s + ")"
    end
  end
end

class DottedReporter < SpecReporter
  def after_it(msg)
    if @report.exception
      if @report.exception.is_a?(ExpectationNotMetError)
        @out.print 'F'
      else
        @out.print 'E'
      end
    else
      @out.print '.'
    end
  end
end

class HtmlReporter < SpecReporter
end

class CIReporter < SpecReporter
  def print_failure(i,r)
    @out.print r.describe + " " + r.it + "\n"
  end
  
  def summary
    unless @summarized
      @exceptions.each_with_index do |r,i|
        print_failure(i+1,r)
      end
      @summarized = true
    end
  end
end

class ImmediateReporter < SpecReporter
  def after_it(msg)
    if @report.exception
      @out.print 'F'
      @out.print "\n" + @report.describe + " " + @report.it + " FAILED\n"
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

class ExpectationNotMetError < StandardError
end

class PositiveExpectation
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    unless @obj == other
      raise ExpectationNotMetError.new("Expected #{@obj.inspect} to equal #{other.inspect}")
    end
  end
  
  def =~(other)
    unless @obj =~ other
      raise ExpectationNotMetError.new("Expected #{@obj.inspect} to match #{other.inspect}")
    end
  end
end

class NegativeExpectation
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    if @obj == other
      raise ExpectationNotMetError.new("Expected #{@obj.inspect} not to equal #{other.inspect}")
    end
  end
  
  def =~(other)
    if @obj =~ other
      raise ExpectationNotMetError.new("Expected #{@obj.inspect} not to match #{other.inspect}")
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

class SpecRunner
  def initialize(reporter=nil)
    @only = []
    @except = []
    @reporter = reporter
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
  end
  
  def reporter
    @reporter
  end
  
  def reporter=(reporter)
    @reporter = reporter
  end
  
  def escape(str)
    str.is_a?(Regexp) ? str : Regexp.new(Regexp.escape(str))
  end
  
  def convert_to_regexps(args)
    args.inject([]) do |list, item|
      if item.is_a?(String) and File.exists?(item)
        if f = File.open(item, "r")
          f.each do |line|
            line.chomp!
            list << escape(line) unless line.empty?
          end
          f.close
        end
        list
      else
        list << escape(item)
      end
    end
  end
  
  def only(*args)
    @only = convert_to_regexps(args)
  end
  
  def except(*args)
    @except = convert_to_regexps(args)
  end
  
  def skip?
    example = @describe.to_s + " " + @it.to_s
    @except.each { |re| return true if re.match(example) }
    return false if @only.empty?
    return true unless @only.any? { |re| re.match(example) }
  end
  
  def before(at=:each,&block)
    if at == :each
      @before.push block
    elsif at == :all
      yield
    else
      raise ArgumentError, "I do not know when you want me to call your block"
    end
  end

  def after(at=:each,&block)
    if at == :each
      @after.push block
    elsif at == :all
      yield
    else
      raise ArgumentError, "I do not know when you want me to call your block"
    end
  end

  def it(msg)
    @it = msg
    return if skip?

    @reporter.before_it(msg)
    begin
      @before.each { |b| b.call }
      yield
      Mock.verify  
    rescue Exception => e
      @reporter.exception(e)
    ensure
      Mock.cleanup
      Mock.reset
      @after.each { |b| b.call }
    end
    @reporter.after_it(msg)
  end

  def describe(msg)
    @before = []
    @after = []
    @describe = msg

    @reporter.before_describe(msg)
    yield
    @reporter.after_describe(msg)
  end
end

if @runner == nil
  @runner = SpecRunner.new
end

# Expose the runner methods
def before(at=:each, &block)
  @runner.before(at, &block)
end

def after(at=:each, &block)
  @runner.after(at, &block)
end

def describe(msg, &block)
  if @runner == self
    raise "fucked!"
  end
  @runner.describe(msg, &block)
end

def it(msg, &block)
  @runner.it(msg, &block)
end

# Alternatives
class Object
  alias context describe
  alias specify it
  alias setup before
  alias teardown after
end
