require 'rubygems'
require 'test/unit'
require 'spec'
$:.unshift(File.dirname(__FILE__) + '/../lib')

class RubiniusTest < Test::Unit::TestCase
  def setup
    setup_machine
  end

  def teardown
  end

  def run(result)
  end

  private
  def rubinius_root
    File.dirname(__FILE__) + '/..'
  end
end

module Kernel
  def context_with_rubinius(name, &block)
    context_without_rubinius name do
      inherit RubiniusTest
      self.class_eval &block
    end
  end
  
  alias context_without_rubinius context
  alias context context_with_rubinius
end