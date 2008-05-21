
$TESTING_MINI_TEST ||= false

#
# Totally minimal (hopefully) drop-in replacement for test/unit
#
#
# DEPRECATED:
#
# :assert_no_match
# :assert_not_equal
# :assert_not_nil
# :assert_not_same
# :assert_nothing_thrown
# :assert_raise

module Mini
  class Assertion < Exception; end

  file = if __FILE__ =~ /^[^\.]/ then # OMG ruby 1.9 is so lame (rubinius too)
           require 'pathname'
           pwd = Pathname.new(Dir.pwd)
           pn = Pathname.new(File.expand_path(__FILE__))
           pn = File.join(".", pn.relative_path_from(pwd)) unless pn.relative?
           pn.to_s
         else
           __FILE__
         end

  MINI_DIR = File.dirname(File.dirname(file))

  def self.filter_backtrace bt
    return ["No backtrace"] unless bt

    new_bt = []
    bt.each do |line|
      break if line.index(MINI_DIR) == 0
      new_bt << line
    end

    new_bt = bt.reject { |line| line.index(MINI_DIR) == 0 } if
      new_bt.empty?
    new_bt = bt.dup if new_bt.empty?

    new_bt
  end

  module Assertions
    def _assertions= n
      @_assertions = n
    end

    def _assertions
      @_assertions ||= 0
    end

    # TODO: refute -> debunk, prove/rebut, show/deny... lots of possibilities
    def assert test, msg = "failed assertion, no message given."
      self._assertions += 1
      raise Mini::Assertion, msg unless test
      true
    end

    def assert_block msg = "assert_block failed."
      assert yield, msg
    end

    def assert_equal exp, act, msg = ""
      msg ||= "" # ARGH! FUCKING RAILS!
      msg += '.' unless msg.empty?
      assert exp == act, "#{msg}\n<#{exp.inspect}> expected, not\n<#{act.inspect}>.".strip
    end

    def assert_in_delta exp, act, delta = 0.001, msg="Expected #{exp} to be within #{delta} of #{act}"
      assert delta.to_f > (exp.to_f - act.to_f).abs, msg
    end

    def assert_includes item, obj, message = "Expected #{item.inspect} to include #{obj.inspect}."
      assert_respond_to obj, :include?
      assert obj.include?(item), message
    end

    def assert_instance_of cls, obj, msg = "Expected #{obj.inspect} to be an instance of #{cls}"
      flip = (Module === obj) && ! (Module === cls) # HACK for specs
      obj, cls = cls, obj if flip
      assert cls === obj, msg
    end

    def assert_kind_of cls, obj, msg = "Expected #{obj.inspect} to be a kind of #{cls}"
      flip = (Module === obj) && ! (Module === cls) # HACK for specs
      obj, cls = cls, obj if flip
      assert obj.kind_of?(cls), msg
    end

    def assert_match exp, act, msg = "Expected #{act.inspect} to match #{exp.inspect}"
      assert act =~ exp, msg
    end

    def assert_nil obj, msg = "Expected #{obj.inspect} to be nil"
      assert obj.nil?, msg
    end

    def assert_nothing_raised _ = :ignored # TODO: deprecate
      self._assertions += 1
      yield
    rescue => e
      raise Mini::Assertion, exception_details(e, "Exception raised:")
    end
    alias :assert_nothing_thrown :assert_nothing_raised

    def assert_operator o1, op, o2, msg = "<#{o1.inspect}> expected to be\n#{op.inspect}\n<#{o2.inspect}>."
      assert o1.__send__(op, o2), msg
    end

    def exception_details e, msg
      "#{msg}\nClass: <#{e.class}>\nMessage: <#{e.message.inspect}>\n---Backtrace---\n#{Mini::filter_backtrace(e.backtrace).join("\n")}\n---------------"
    end

    def assert_raises *exp
      msg = String === exp.last ? exp.pop : nil
      should_raise = false
      begin
        yield
        should_raise = true
      rescue Exception => e
        assert_includes e.class, exp
        exception_details(e, "<#{exp.inspect}> exception expected, not")
        return e
      end

      exp = exp.first if exp.size == 1
      flunk "#{exp.inspect} expected but nothing was raised." if should_raise
    end
    alias :assert_raise :assert_raises

    def assert_respond_to obj, meth, msg = "Expected #{obj.inspect} of type #{obj.class} to respond_to? #{meth.inspect}"
      flip = (Symbol === obj) && ! (Symbol === meth) # HACK for specs
      obj, meth = meth, obj if flip
      assert obj.respond_to?(meth), msg
    end

    def assert_same exp, act, msg = "Expected #{act.inspect} to be the same as #{exp.inspect}."
      assert exp.equal?(act), msg
    end

    def assert_throws sym, msg = "<#{sym.inspect}> should have been thrown"
      caught = true
      catch(sym) do
        begin
          yield
        rescue ArgumentError => e     # 1.9 exception
          msg += ", not <#{e.message.split(/ /).last}>"
        rescue NameError => e         # 1.8 exception
          msg += ", not <#{e.name.inspect}>"
        end
        caught = false
      end
      assert caught, msg + "."
    end

    def flunk msg = "Flunked"
      assert false, msg
    end

    def refute test, msg = "failed refutation, no message given."
      not assert(! test, msg)
    end

    def refute_equal exp, act, msg = "Expected #{act.inspect} to not be equal to #{exp.inspect}"
      refute exp == act, msg
    end
    alias :assert_not_equal :refute_equal

    def refute_match exp, act, msg = "Expected #{act.inspect} to not match #{exp.inspect}"
      refute act =~ exp, msg
    end
    alias :assert_no_match :refute_match

    def refute_nil obj, msg = "Expected #{obj.inspect} to not be nil"
      refute obj.nil?, msg
    end
    alias :assert_not_nil :refute_nil

    def refute_same exp, act, msg = "Expected #{act.inspect} to not be the same as #{exp.inspect}"
      refute exp.equal?(act), msg
    end
    alias :assert_not_same :refute_same
  end

  class Test
    VERSION = "1.2.0"

    attr_reader :report, :failures, :errors

    @@installed_at_exit ||= false
    @@out = $stdout

    def self.autorun
      at_exit {
        exit_code = Mini::Test.new.run(ARGV)
        exit exit_code if exit_code
      } unless @@installed_at_exit
      @@installed_at_exit = true
    end

    def self.output= stream
      @@out = stream
    end

    def puke klass, meth, e
      if Mini::Assertion === e then
        @failures += 1

        loc = e.backtrace.find { |s| s !~ /in .(assert|flunk|raise)/ }
        loc.sub!(/:in .*$/, '')

        @report << "Failure:\n#{meth}(#{klass}) [#{loc}]:\n#{e.message}\n"
        'F'
      else
        @errors += 1
        bt = Mini::filter_backtrace(e.backtrace).join("\n    ")
        e = "Error:\n#{meth}(#{klass}):\n#{e.class}: #{e.message}\n    #{bt}\n"
        @report << e
        'E'
      end
    end

    def initialize
      @report = []
      @errors = @failures = 0
    end

    ##
    # Top level driver, controls all output and filtering.

    def run args
      filter = if args.first =~ /^(-n|--name)$/ then
                 args.shift
                 arg = args.shift
                 arg =~ /\/(.*)\// ? Regexp.new($1) : arg
               else
                 /./ # anything - ^test_ already filtered by #tests
               end

      @@out.puts "Loaded suite #{$0.sub(/\.rb$/, '')}\nStarted"

      start = Time.now
      run_test_suites filter

      @@out.puts
      @@out.puts "Finished in #{'%.6f' % (Time.now - start)} seconds."

      @report.each_with_index do |msg, i|
        @@out.puts "\n%3d) %s" % [i + 1, msg]
      end

      @@out.puts

      format = "%d tests, %d assertions, %d failures, %d errors"
      @@out.puts format % [@test_count, @assertion_count, failures, errors]

      return failures + errors if @test_count > 0 # or return nil...
    end

    def run_test_suites filter = /^test/
      @test_count, @assertion_count = 0, 0
      TestCase.test_suites.each do |suite|
        suite.test_methods.grep(filter).each do |test|
          inst = suite.new test
          inst._assertions = 0
          @@out.puts "\n#{test}: " if $DEBUG
          result = '.'
          begin
            inst.setup
            inst.__send__ test
          rescue Exception => e
            result = puke(suite, test, e)
          ensure
            begin
              inst.teardown
            rescue Exception => e
              result = puke(suite, test, e)
            end
          end
          @@out.print result
          @@out.puts if $DEBUG
          @test_count += 1
          @assertion_count += inst._assertions
        end
      end
      [@test_count, @assertion_count]
    end

    class TestCase
      attr_reader :name

      def initialize name
        @name = name
      end

      def self.reset
        @@test_suites = {}
      end

      reset

      def self.inherited klass
        @@test_suites[klass] = true
      end

      def self.test_suites
        @@test_suites.keys.sort_by { |ts| ts.name }
      end

      def self.test_methods
        public_instance_methods(true).grep(/^test/).sort.map { |m| m.to_s }
      end

      def setup; end
      def teardown; end

      include Mini::Assertions
    end # class TestCase
  end # class Test
end # module Mini
