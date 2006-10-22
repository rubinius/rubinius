require 'translation/extractors'
require 'test/unit'

class TestExtractClasses # < Test::Unit::TestCase
  def setup
    @ec = ExtractClasses.new
  end
  
  def test_process
    sx = [:class, [:colon2, :Box], nil, [:scope, [:block]]]
    @ec.process sx
    assert_kind_of Type::Klass, @ec.classes[:Box]
    assert_equal @ec.classes[:Box].superklass, :Object
  end
  
  def test_find_methods
    sx = [:class, [:colon2, :Box], nil, [:scope, [:block, 
      [:defn, :go, [:scope, [:block, [:args], [:true]]]]
    ]]]
    
    @ec.process sx
    box = @ec.classes[:Box]
    assert_equal [:go], box.defined_methods.keys
  end
  
  def test_find_ivars
    sx = [:class, [:colon2, :Box], nil, [:scope, [:block, 
      [:defn, :go, [:scope, [:block, [:args], [:iasgn, :@age, [:lit, 9]]]]]
    ]]]
    
    @ec.process sx
    box = @ec.classes[:Box]
    assert_equal [:age], box.ivars.keys
        
  end
end

require 'tempfile'

class TesExtractRequires < Test::Unit::TestCase
  def test_process
    sx = [:scope, [:block, [:call, [:self], :require, [:array, [:str, "blah.rb"]]]]]
    pr = ExtractRequires.new
    pr.process sx
    assert_equal ["blah.rb"], pr.requires
  end
  
  def notest_extract_recursive
    t1 = Tempfile.new("rbs")
    t2 = Tempfile.new("rbs")
    t3 = Tempfile.new("rbs")
    
    t1 << "require '#{t2.path}'\n"
    t2 << "require '#{t3.path}'\n"
    t3 << "1 + 1\n"
    
    t1.close
    t2.close
    t3.close
    
    lst = ExtractRequires.from_file(t1.path)
    
    assert_equal [t3.path, t2.path, t1.path], lst
  end
end

class TestExtractHints < Test::Unit::TestCase
  
  def test_process_for_type_outside_class_is_masked
    sx = [:block, 
      [:comment, " T:String => String"],
      [:class, [:colon2, :Box], nil, [:scope, [:block, 
        [:defn, :go, [:args], [:scope, [:block,
          [:true]
        ]]]
      ]]]
    ]
    
    ex = ExtractHints.new
    ex.process sx
    hint = ex.hints[[:Box, :go]]
    assert_equal nil, hint
  end
  
  def test_process_for_type
    sx = [:block, 
      [:class, [:colon2, :Box], nil, [:scope, [:block, 
        [:comment, " T:String => String"],
        [:defn, :go, [:args], [:scope, [:block,
          [:true]
        ]]]
      ]]]
    ]
    
    ex = ExtractHints.new
    ex.process sx
    hint = ex.hints[[:Box, :go]]
    assert_equal [:hint, :type, Type.String, [Type.String]], hint
  end
  
  def test_process_for_tag
    sx = [:block, 
      [:class, [:colon2, :Box], nil, [:scope, [:block, 
        [:comment, "hints:rocks"],
        [:defn, :go, [:args], [:scope, [:block,
          [:true]
        ]]]
      ]]]
    ]
    
    ex = ExtractHints.new
    ex.process sx
    hint = ex.hints[[:Box, :go]]
    assert_equal [:hint, :tag, ["rocks"]], hint
  end
  
  def test_process_using_external_comments
    sx = [:block, 
      [:class, [:colon2, :Box], nil, [:scope, [:block, 
        [:defn, :go, [:args], [:scope, [:block,
          [:true]
        ]], 3]
      ]]]
    ]
    
    coms = [[2, 1, " hints:rocks"]]
    
    ex = ExtractHints.new(coms, true)
    ex.process sx
    hint = ex.hints[[:Box, :go]]
    assert_equal [:hint, :tag, ["rocks"]], hint    
  end
end