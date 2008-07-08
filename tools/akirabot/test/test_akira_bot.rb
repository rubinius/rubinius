#!/usr/bin/env ruby -w

require 'test/unit'
require 'stringio'

require 'akira_bot'

class TestAkiraBot < Test::Unit::TestCase
  def setup
    @bot = AkiraBot.new :channel => "#channel"
    @bot.socket = @io = StringIO.new
  end

  def test_sanity
    @bot.say "text"

    assert_equal "PRIVMSG #channel :text\r\n", @io.string
  end
end
