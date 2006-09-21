require 'rubygems'
require 'test/unit'
require 'translation/rbs_translate'

class TestRubiniusTranslator < Test::Unit::TestCase
  
  Code1 = <<-CODE
  class Box
    attr_accessor :age
    
    def go
      @age + 1
    end
    
    def initialize
      @age = 1
    end
  end
  CODE
  
  def test_translate
    
    exc_header = <<-CODE
struct rbc_Box {
  int age;
};

int rbc_Box_go(struct rbc_Box *self);
int rbc_Box_initialize(struct rbc_Box *self);
struct rbc_Box *rbc_Box_new();
CODE

    exc_code = <<-CODE
struct rbc_Box *rbc_Box_new() {
  struct rbc_Box *self;
  self = (struct rbc_Box*)malloc(sizeof(struct rbc_Box));
  rbc_Box_initialize(self);
  return self;
}

int rbc_Box_go(struct rbc_Box *self) {
  return ({ self->age + 1; });
}

int rbc_Box_initialize(struct rbc_Box *self) {
  return self->age = 1;
}
CODE

    trans = RubiniusTranslator.new("rbc_")
    trans.load_types
    trans.load Code1

    assert [:Box], trans.classes.keys
    assert [:go, :initialize], trans.classes[:Box].functions.keys
    
    trans.translate_to_c
    assert_equal exc_header, trans.header
    assert_equal exc_code, trans.body
  end
  
  Code2 = <<-CODE
    class DeliveryMan
      def initialize
        @box = Box.new
      end
      
      def test
        g = @box.go
        return g
      end
      
      def package
        i = @box
        return i
      end
    end
  CODE
  
  require 'pp'
  
  def test_translate_multiple_code
    trans = RubiniusTranslator.new("rbc_")
    trans.load_types
    trans.load Code2
    trans.load Code1
    trans.rehash_functions
    
    dm = trans.classes[:DeliveryMan]
    assert_equal Type.Fixnum, dm.functions[:test].type
    assert_equal Type.Box, dm.functions[:package].type
    
  end
end