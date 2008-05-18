require 'mini/test'

module Test
  module Unit # was ::Mini::Test, but rails' horrid code forced my hand

    remove_const :TestCase if defined? TestCase # nope... fuck you
    TestCase = ::Mini::Test::TestCase

    class TestCase
      alias :method_name :name # so lame
    end

    AssertionFailedError = ::Mini::Assertion

    class TestCase
      def build_message(user_message, template_message, *args)
        user_message ||= ''
        user_message += ' ' unless user_message.empty?
        msg = template_message.split(/<\?>/).zip(args.map { |o| o.inspect })
        user_message + msg.join
      end
    end
  end
end

Mini::Test.autorun unless $TESTING_MINI_TEST
