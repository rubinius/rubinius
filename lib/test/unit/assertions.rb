require 'mini/test'
require 'test/unit'

module Test::Unit # patch up bastards that that extend improperly.
  if defined? Assertions then
    warn "ARGH! someone defined Test::Unit::Assertions rather than requiring"
    CRAP_ASSERTIONS = Assertions
    remove_const :Assertions

    ObjectSpace.each_object(Module) do |offender|
      offender.send :include, ::Mini::Assertions if offender < CRAP_ASSERTIONS
    end rescue :fuck_you_then!

    Test::Unit::TestCase.send :include, CRAP_ASSERTIONS
  end

  Assertions = ::Mini::Assertions

  module Assertions
    def self.included x
      x.send :include, Test::Unit::CRAP_ASSERTIONS
    end if defined? Test::Unit::CRAP_ASSERTIONS
  end
end

class Module # define deprecation api
  DEPS = Hash.new { |h,k| h[k] = {} }

  def deprecation_warning old, new = nil, kaller = nil
    kaller ||= caller[1]
    unless DEPS[old][kaller] then
      msg = "#{self}##{old} deprecated. "
      msg += new ? "Use ##{new}" : "No replacement is provided"
      msg += ". From #{kaller}."
      warn msg
    end
    DEPS[old][kaller] = true
  end

  def deprecate old, new
    class_eval <<-EOM
      def #{old} *args, &block
        cls, clr = self.class, caller.first
        self.class.deprecation_warning #{old.inspect}, #{new.inspect}, clr
        #{new}(*args, &block)
      end
    EOM
  end
end

module Test::Unit
  module Assertions # deprecations
    deprecate :assert_nothing_thrown, :assert_nothing_raised # drop 2009-06-01
    deprecate :assert_raise,          :assert_raises         # drop 2010-06-01
    deprecate :assert_not_equal,      :refute_equal          # drop 2009-06-01
    deprecate :assert_no_match,       :refute_match          # drop 2009-06-01
    deprecate :assert_not_nil,        :refute_nil            # drop 2009-06-01
    deprecate :assert_not_same,       :refute_same           # drop 2009-06-01

    def assert_nothing_raised _ = :ignored                   # drop 2009-06-01
      self.class.deprecation_warning :assert_nothing_raised
      self._assertions += 1
      yield
    rescue => e
      raise Mini::Assertion, exception_details(e, "Exception raised:")
    end

    def build_message(user_message, template_message, *args) # drop 2009-06-01
      user_message ||= ''
      user_message += ' ' unless user_message.empty?
      msg = template_message.split(/<\?>/).zip(args.map { |o| o.inspect })
      user_message + msg.join
    end
  end
end
