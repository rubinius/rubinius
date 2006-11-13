require 'test/unit'
require 'cuby'

class TestCuby < Test::Unit::TestCase
  def setup
    @c = Cuby.new
  end
  
  def test_fcall
    code = [:fcall, :stack_pop]
    @c.declare_function 'object', 'stack_pop', []
    @c.generate_from code
    assert_equal @c.code, "stack_pop()"
    
    @c.reset
    
    @c.declare_function 'object', 'stack_push', ['int']
    code = [:fcall, :stack_push, [:array, [:lit, 2]]]
    @c.generate_from code.deep_clone
    assert_equal "stack_push(2)", @c.code
  end
  
  def test_fcall_error
    code = [:fcall, :stack_pop]
    assert_raises(Cuby::Error) do
      @c.generate_from code
    end
  end
  
  def test_vcall
    code = [:vcall, :stack_pop]
    @c.declare_function 'object', 'stack_pop', []
    @c.generate_from code
    assert_equal @c.code, "stack_pop()"
  end
  
  def test_lasgn
    code = [:lasgn, :i, 2, [:fcall, :stack_pop]]
    @c.declare_function 'object', 'stack_pop', []
    @c.declare_var 'object', 'i'
    @c.generate_from code
    assert_equal @c.code, "i = stack_pop()"
  end
  
  def test_lvar
    code = [:lvar, :o, 2]
    @c.declare_var 'object', 'o'
    @c.generate_from code
    assert_equal @c.code, "o"
  end
  
  def test_block
    code = [:block, [:fcall, :stack_pop]]
    @c.declare_function 'object', 'stack_pop', []
    @c.generate_from code
    assert_equal @c.code, "stack_pop();\n"
  end
  
  def test_lit
    code = [:lit, 3]
    @c.generate_from code
    assert_equal @c.code, "3"
  end
  
  def test_or
    code = [:or, [:lit, 1], [:lit, 3]]
    @c.generate_from code
    assert_equal @c.code, "(1 || 3)"
  end
  
  def test_and
    code = [:and, [:lit, 1], [:lit, 3]]
    @c.generate_from code
    assert_equal "(1 && 3)", @c.code
  end
  
  def test_not
    code = [:not, [:lit, 0]]
    @c.generate_from code
    assert_equal @c.code, "!0"
  end
  
  def test_false
    code = [:false]
    @c.false_value = "FALSE"
    @c.generate_from code
    assert_equal @c.code, "FALSE"
  end
  
  def test_true
    code = [:true]
    @c.true_value = "TRUE"
    @c.generate_from code
    assert_equal @c.code, "TRUE"
  end
  
  def test_break
    code = [:break]
    @c.generate_from code
    assert_equal @c.code, "break"
  end
  
  def test_next
    code = [:next]
    @c.generate_from code
    assert_equal @c.code, "continue"
  end
  
  def test_operator
    @c.operators << :+
    code = [:call, [:lit, 1], :+, [:array, [:lit, 2]]]
    @c.generate_from code
    assert_equal @c.code, "1 + 2"
    
    assert_raises(Cuby::UnknownMethod) do
      code = [:call, [:lit, 1], :-, [:array, [:lit, 2]]]
      @c.generate_from code
    end
  end
  
  def test_add_method
    @c.add_method(:fixnum?) do |me|
      "FIXNUM_P(#{me})"
    end
    
    code = [:call, [:lit, 1], :fixnum?, [:array]]
    @c.generate_from code
    assert_equal @c.code, "FIXNUM_P(1)"
  end
  
  def assert_not(cond)
    assert !cond, "Expected to be false, but was true."
  end
  
  def test_declare_var
    @c.declare_var "int", "a"
    var = @c.variables[:a]
    assert var
    assert_equal "int", var.type
    assert_not var.pointer?
    assert_not var.struct?
    
    @c.declare_var "struct blah", "b"
    var = @c.variables[:b]
    assert var
    assert_equal "struct blah", var.type
    assert_not var.pointer?
    assert var.struct?
    assert_equal :blah, var.struct_name
    
    @c.declare_var "struct blah*", "c"
    var = @c.variables[:c]
    assert_equal "struct blah", var.type
    assert var.pointer?
    assert var.struct?
    assert_equal :blah, var.struct_name
    
    @c.declare_var "struct blah", "*d"
    assert @c.variables[:d].pointer?
  end
  
  def test_struct
    @c.add_struct(:cpu) do |s|
      s.add_field "int", :ip
    end
    
    @c.declare_var "struct cpu", "c"
    assert_equal @c.structs[:cpu], @c.resolve_struct(:c)
    
    code = [:call, [:lvar, :c, 2], :ip=, [:array, [:lit, 3]]]
    @c.generate_from code
    assert_equal "c.ip = 3", @c.code
    assert_equal "int", @c.last_type
    
    @c.reset
    
    code = [:call, [:lvar, :c, 2], :ip, [:array]]
    @c.generate_from code
    assert_equal "c.ip", @c.code
    assert_equal "int", @c.last_type
    
    code = [:call, [:lvar, :c, 2], :unk=, [:array, [:lit, 3]]]
    assert_raises(Cuby::Error) do
      @c.generate_from code
    end
    
    code = [:call, [:lvar, :c, 2], :unk, [:array, [:lit, 3]]]
    assert_raises(Cuby::Error) do
      @c.generate_from code
    end
    
    @c.reset
    
    @c.declare_var "struct cpu", "*d"
    code = [:call, [:lvar, :d, 2], :ip=, [:array, [:lit, 3]]]
    @c.generate_from code
    assert_equal "d->ip = 3", @c.code
    assert_equal "int", @c.last_type
    
    @c.reset
    
    code = [:call, [:lvar, :d, 2], :ip, [:array]]
    @c.generate_from code
    assert_equal "d->ip", @c.code
    assert_equal "int", @c.last_type
    
    @c.reset
    
    code = [:call, [:lvar, :d, 2], :unk=, [:array, [:lit, 3]]]
    assert_raises(Cuby::Error) do
      @c.generate_from code
    end
    
    code = [:call, [:lvar, :d, 2], :unk, [:array, [:lit, 3]]]
    assert_raises(Cuby::Error) do
      @c.generate_from code
    end
  end
  
  def test_if
    code = [:if, [:lit, 1], [:lit, 3], [:lit, 4]]
    @c.generate_from code
    assert_equal "if(1) {\n3;\n} else {\n4;\n}", @c.code
    
    @c.reset
    
    code = [:if, [:lit, 1], [:block, [:lit, 50]]]
    @c.generate_from code
    assert_equal "if(1) {\n50;\n}", @c.code
    
    @c.reset
    
    code = [:if, [:lit, 1], nil, [:lit, 70]]
    @c.generate_from code
    assert_equal "if(!(1)) {\n70;\n}", @c.code
    
    @c.reset
    
    code = [:if, [:lit, 1], nil, nil]
    @c.generate_from code
    assert_equal "if(1) {}", @c.code
  end
  
  def test_while
    code = [:while, [:lit, 1], [:lit, 50], false]
    @c.generate_from code
    assert_equal "while(1) {\n50;\n}", @c.code
    
    @c.reset
    
    code = [:while, [:lit, 1], [:lit, 50], true]
    @c.generate_from code
    assert_equal "do {\n50;\n} while(1);", @c.code
  end
  
  def test_until
    code = [:until, [:lit, 1], [:lit, 1], false]
    @c.generate_from code
    assert_equal "while(!(1)) {\n1;\n}", @c.code
    
    @c.reset
    
    code = [:until, [:lit, 1], [:lit, 1], true]
    @c.generate_from code
    assert_equal "do {\n1;\n} while(!(1));", @c.code
  end
  
  def test_apply_methods
    
    @c.declare_function "ulong", "find_class", ["int", "int", "int"]
    
    map = {
      :class => "find_class",
      :object_id => "calculate_object_id"
    }
    
    @c.map_operator = :as
    @c.add_map :object, map
    @c.declare_var "int", "c"
    
    code = [:block,
      [:call, [:lvar, :c, 2], :as, [:array, [:lit, :object]]],
      [:call, [:lvar, :c, 2], :class, [:array]]
    ]
    
    @c.generate_from code
    assert_equal "find_class(c);\n", @c.code
    assert_equal "ulong", @c.last_type
    
    @c.reset
    
    code = [:block,
      [:call, [:lvar, :c, 2], :as, [:array, [:lit, :object]]],
      [:call, [:lvar, :c, 2], :class, [:array, [:lit, 1], [:lit, 2]]]
    ]
    
    @c.generate_from code
    assert_equal "find_class(c, 1, 2);\n", @c.code
    assert_equal "ulong", @c.last_type
    
    @c.reset
    
    code = [:block,
      [:call, [:lvar, :c, 2], :as, [:array, [:lit, :object]]],
      [:call, [:lvar, :c, 2], :unknown, [:array, [:lit, 1], [:lit, 2]]]
    ]
    
    assert_raises(Cuby::UnknownMethod) do
      @c.generate_from code
    end
    
  end
  
  def test_expanding_applied_method
    @c.declare_function "ulong", "find_class", ["object", "object"]
    map = {
      :class => "find_class(state, %s)"
    }
    
    @c.map_operator = :as
    @c.add_map :object, map
    @c.declare_var "object", "c"
    
    code = [:block,
      [:call, [:lvar, :c, 2], :as, [:array, [:lit, :object]]],
      [:call, [:lvar, :c, 2], :class, [:array]]
    ]
    
    @c.generate_from code
    assert_equal "find_class(state, c);\n", @c.code
  end
  
  def test_mapping_errors
    code = [:call, [:lit, 10], :as, [:array, [:lit, :object]]]
    assert_raises(Cuby::UnknownMethod) do
      @c.generate_from code
    end
    
    code = [:call, [:lvar, :c, 2], :as, [:array, [:lit, :object]]]
    assert_raises(Cuby::Error) do
      @c.generate_from code
    end
  end
  
  def test_implicit_literal_maps
    
    @c.declare_function "int", "sqrt", ["int"]
    
    assert_equal :numeric, @c.calculate_literal_map(10)
    
    map = {
      :sqrt => "sqrt"
    }
    
    @c.add_map :numeric, map
    code = [:call, [:lit, 2], :sqrt, [:array]]
    
    @c.generate_from code
    assert_equal "sqrt(2)", @c.code
    assert_equal "int", @c.last_type
    
    code = [:call, [:lit, 2], :unknown, [:array]]
    assert_raises(Cuby::UnknownMethod) do
      @c.generate_from code
    end
    
    assert_equal :string, @c.calculate_literal_map("hello")
    code = [:call, [:str, "blah"], :dup, [:array]]
    assert_raises(Cuby::UnknownMethod) do
      @c.generate_from code.deep_clone
    end
    
    @c.reset
    
    @c.add_map :string, :dup => "string_dup"
    @c.generate_from code
    assert_equal 'string_dup("blah")', @c.code
  end
  
  def test_lvar_map_by_type
    @c.add_map :numeric, :sqrt => "sqrt"
    @c.add_type_map "int", :numeric
    var = @c.declare_var "int", "c"
    assert var.find_mapping(:sqrt)
    
    code = [:call, [:lvar, :c, 2], :sqrt, [:array]]
    @c.generate_from code
    assert_equal "sqrt(c)", @c.code
  end
  
  def test_map_by_last_type
    @c.declare_function "int", "sqrt", ["int"]
    @c.add_type_map "int", :numeric
    @c.add_map :numeric, :sqrt => "sqrt"
    code = [:call, [:call, [:lit, 88], :sqrt, [:array]], :sqrt, [:array]]
    @c.generate_from code
    assert_equal "sqrt(sqrt(88))", @c.code
  end
  
  def test_str
    code = [:str, "blah"]
    @c.generate_from code
    assert_equal '"blah"', @c.code
  end
  
  def test_last_type
    @c.declare_var "int", "c"
    
    code = [:lvar, :c, 2]
    @c.generate_from code
    assert_equal "int", @c.last_type
    
    @c.reset
    
    @c.declare_function "long", "stack_pop", []
    code = [:fcall, :stack_pop]
    @c.generate_from code
    assert_equal "long", @c.last_type
    
    @c.reset
    
    @c.declare_function "short", "blah", []
    code = [:vcall, :blah]
    @c.generate_from code
    assert_equal "short", @c.last_type
    
    @c.reset
    
    @c.set_literal_type Fixnum, "int"
    code = [:lit, 10]
    @c.generate_from code
    assert_equal "int", @c.last_type
  end
  
  def test_self
    code = [:self]
    assert_raises(Cuby::Error) do
      @c.generate_from code  
    end
    
    @c.declare_var "object", "self"
    @c.generate_from [:self]
    assert_equal "self", @c.code
  end
  
  def test_random_concrete_examples
    @c.declare_var "object", "self"
    @c.declare_var "object", "methctx"
    @c.add_type_map "object", :object
    @c.add_map :object, :kind_of? => "RISA"
    
    code = [:not, [:call, [:lvar, :self, 2], :kind_of?, [:array, [:lvar, :methctx, 3]]]]
    @c.generate_from code
    output = "!RISA(self, methctx)"
    assert_equal output, @c.code
  end
  
end