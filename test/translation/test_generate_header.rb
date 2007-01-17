require File.expand_path(File.dirname(__FILE__) + '/../test_helper')
require 'translation/generate_header'

class TestGenerateHeader < Test::Unit::TestCase
  def setup
    
    @ti = TypeInfo.new("rbs_test_")
    @ti.add_c_type Type.Fixnum, "int"
    
    @klass = @ti.add_klass(:Box, :Object)
    @fun = Function.new(:go, [], [])
    @fun.type = Type.Fixnum
    @klass.defined_methods[:go] = @fun
    @klass.ivars[:age] = Type.Fixnum
    @gh = GenerateHeader.new(@klass, @ti)
    @gh.prefix = "rbs_test_"
  end
  
  def test_generate
    str = @gh.generate
    exc = <<-CODE
struct rbs_test_Box {
  int age;
};

int rbs_test_Box_go(struct rbs_test_Box *self);
struct rbs_test_Box *rbs_test_Box_new();
CODE
    
    assert_equal exc, str
  end
  
  def test_generate_method_with_args
    @ti.add_c_type Type.Array, "ARRAY"
    @fun.args = [Type.Array, Type.Array]
    @fun.arg_names = [:persons, :groups]
    str = @gh.generate
    exc = <<-CODE
struct rbs_test_Box {
  int age;
};

int rbs_test_Box_go(struct rbs_test_Box *self, ARRAY persons, ARRAY groups);
struct rbs_test_Box *rbs_test_Box_new();
CODE

    assert_equal exc, str
  end
  
  def test_generate_with_superclass
    sup = @ti.add_klass(:Container, :Object)
    sup.ivars[:destination] = Type.String
    @klass.ivars[:weight] = Type.Fixnum
    @klass.superklass = :Container
    @ti.add_c_type Type.String, "STRING"
    
    str = @gh.generate
    exc = <<-CODE
struct rbs_test_Box {
  STRING destination;
  int age;
  int weight;
};

int rbs_test_Box_go(struct rbs_test_Box *self);
struct rbs_test_Box *rbs_test_Box_new();
CODE
    
    assert_equal exc, str
  end
end