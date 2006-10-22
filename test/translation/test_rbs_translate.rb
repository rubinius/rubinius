require 'rubygems'
require 'test/unit'
require 'translation/rbs_translate'

class TestRubiniusTranslator < Test::Unit::TestCase
  
  def setup
    TypeInfo::CodeGenerator.reset
  end
  
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
  
  Code3 = <<-CODE
   class Box
     def self.ups
       1
     end
     
     def initialize
       @kind = 1
     end
   end
  CODE
  
  def test_translate_of_defs
    trans = RubiniusTranslator.new("rbc_")
    trans.load_types
    trans.load Code3
    trans.translate_to_c
    
    exc_header = <<-CODE
struct rbc_Box {
  int kind;
};

int rbc_Box_initialize(struct rbc_Box *self);
struct rbc_Box *rbc_Box_new();
int rbc_Box_s_ups();
CODE
    
    assert_equal exc_header, trans.header
    
    exc_body = <<-CODE
struct rbc_Box *rbc_Box_new() {
  struct rbc_Box *self;
  self = (struct rbc_Box*)malloc(sizeof(struct rbc_Box));
  rbc_Box_initialize(self);
  return self;
}

int rbc_Box_initialize(struct rbc_Box *self) {
  return self->kind = 1;
}

int rbc_Box_s_ups() {
  return 1;
}
CODE
    
    assert_equal exc_body, trans.body
  end
    
  Code4 = <<-CODE
    class Box
      def initialize(index)
        @index = index
        return 0
      end
      
      def self.train_initialize
        Box.new(1)
      end
    end
  CODE
  
  def test_translation_of_new
    trans = RubiniusTranslator.new("rbc_")
    trans.load_types
    trans.load Code4
    trans.translate_to_c
    
    exc_header = <<-CODE
struct rbc_Box {
  int index;
};

int rbc_Box_initialize(struct rbc_Box *self, int index);
struct rbc_Box *rbc_Box_new(int index);
struct rbc_Box* rbc_Box_s_train_initialize();
    CODE
    
    assert_equal exc_header, trans.header
    
    exc_body = <<-CODE
struct rbc_Box *rbc_Box_new(int index) {
  struct rbc_Box *self;
  self = (struct rbc_Box*)malloc(sizeof(struct rbc_Box));
  rbc_Box_initialize(self, index);
  return self;
}

struct _locals1 {
  int index;
};

int rbc_Box_initialize(struct rbc_Box *self, int index) {
  struct _locals1 _locals, *locals = &_locals;
  locals->index = index;
  self->index = locals->index;
  return 0;
}

struct rbc_Box* rbc_Box_s_train_initialize() {
  return rbc_Box_s_new(1);
}
    CODE
    assert_equal exc_body, trans.body
  end
  
  Code6 = <<-CODE
  class Box
    # T:Fixnum => Fixnum
    def start(idx)
      idx
    end
  end
  CODE
  
  def test_translation_with_comments
    trans = RubiniusTranslator.new("rbc_")
    trans.load_types
    trans.load Code6
    
    trans.translate_to_c

    exc_header = <<-CODE
struct rbc_Box {

};

int rbc_Box_start(struct rbc_Box *self, int idx);
struct rbc_Box *rbc_Box_new();
    CODE
    
    exc_code = <<-CODE
struct rbc_Box *rbc_Box_new() {
  struct rbc_Box *self;
  self = (struct rbc_Box*)malloc(sizeof(struct rbc_Box));
  return self;
}

struct _locals1 {
  int idx;
};

int rbc_Box_start(struct rbc_Box *self, int idx) {
  struct _locals1 _locals, *locals = &_locals;
  locals->idx = idx;
  return locals->idx;
}
    CODE
    
    assert_equal exc_header, trans.header
    assert_equal exc_code, trans.body
  end
end