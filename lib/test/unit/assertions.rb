require 'mini/test'
require 'test/unit'

module Test::Unit
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

    def build_message(user_message, template_message, *args)
      user_message ||= ''
      user_message += ' ' unless user_message.empty?
      msg = template_message.split(/<\?>/).zip(args.map { |o| o.inspect })
      user_message + msg.join
    end
  end
end
