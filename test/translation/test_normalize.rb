require 'test/unit'
require 'translation/normalize'

class TestRsNormalize < Test::Unit::TestCase
  
  def setup
    @rs = RsNormalizer.new  
  end
  
  def norm(x)
    @rs.process x.deep_clone
  end
  
  def test_normalize_iter
    input = [:iter, 
      [:call, [:self], :blah, [:array]], 
      nil,
      [:call, [:self], :go, [:array]]]
      
    output = [:iter, 
      [:call, [:self], :blah, [:array]], 
      nil,
      [:block, [:call, [:self], :go, [:array]]]]
    
    got = norm input
    
    assert_equal output, got
    
    got2 = norm output
    
    assert_equal output, got2
  end
  
  def test_process_call
    input = [:call, [:self], :blah]
    output = [:call, [:self], :blah, [:array]]
    
    got = norm input
    assert_equal output, got
    
    got = norm output
    assert_equal output, got
  end
  
  def test_process_vcall
    input = [:vcall, :blah]
    output = [:call, [:self], :blah, [:array]]
    
    got = norm input
    assert_equal output, got
  end
  
  def test_process_fcall
    input = [:fcall, :blah]
    output = [:call, [:self], :blah, [:array]]
    
    got = norm input
    assert_equal output, got
    
    input = [:fcall, :blah, [:array]]
    output = [:call, [:self], :blah, [:array]]
    
    got = norm input
    assert_equal output, got
    
  end
      
  def test_process_zarray
    input = [:zarray]
    output = [:array]
    
    got = norm input
    assert_equal output, got
  end
  
  def test_process_defn
    input = [:defn, :go, [:scope, 
      [:block,
        [:args, [:a, :b, :c], [], nil, nil],
        [:true]
      ]
    ]]
    
    output = [:defn, :go, [:args, [:a, :b, :c], [], nil, nil],
      [:scope, 
        [:block,
          [:true]
        ]
    ]]
    
    got = norm input
    assert_equal output, got  
    
  end
  
  def test_process_defn_with_block_arg
    input = [:defn, :go, [:scope, 
      [:block,
        [:args, [:a, :b, :c], [], nil, nil],
        [:block_arg, :d, 5],
        [:true]
      ]
    ]]
    
    output = [:defn, :go, [:args, [:a, :b, :c], [], nil, nil, [:block_arg, :d, 5]],
      [:scope, 
        [:block,
          [:true]
        ]
    ]]
    
    got = norm input
    assert_equal output, got  
    
  end
  
  
  def test_process_if
    input = [:if, [:true], [:lit, 8], [:lit, 9]]
    output = [:if, [:true], [:block, [:lit, 8]], [:block, [:lit, 9]]]
    
    got = norm input
    assert_equal output, got
    
    input = [:if, [:true], [:lit, 8], nil]
    output = [:if, [:true], [:block, [:lit, 8]], nil]
    
    got = norm input
    assert_equal output, got
  end
  
  def test_process_while
    input =  [:while, [:true], [:lit, 8]]
    output = [:while, [:true], [:block, [:lit, 8]]]
    
    got = norm input
    assert_equal output, got
  end
  
  def test_process_until
    input =  [:until, [:true], [:lit, 8]]
    output = [:until, [:true], [:block, [:lit, 8]]]
    
    got = norm input
    assert_equal output, got
  end
  
  def test_process_when
    input =  [:when, [:array, [:lit, 1]], [:true]]
    output = [:when, [:array, [:lit, 1]], [:block, [:true]]]
    
    got = norm input
    assert_equal output, got
  end
  
  def test_process_case
    input = [:case, [:lit, 1], 
      [
        [:when, [:array, [:lit, 1]], [:vcall, :blah]],
        [:when, [:array, [:lit, 99]], [:vcall, :bleh]]
      ]
    ]
    
    output = [:case, [:lit, 1], 
      [
        [:when, [:array, [:lit, 1]], [:block, [:call, [:self], :blah, [:array]]]],
        [:when, [:array, [:lit, 99]], [:block, [:call, [:self], :bleh, [:array]]]]
      ],
      nil
    ]
    
    
    got = norm input
    assert_equal output, got
  end

  def test_process_case_with_else
    input = [:case, [:lit, 1], 
      [
        [:when, [:array, [:lit, 1]], [:vcall, :blah]],
        [:when, [:array, [:lit, 99]], [:vcall, :bleh]],
        [:else, [:vcall, :bluh]]
      ]
    ]
    
    output = [:case, [:lit, 1], 
      [
        [:when, [:array, [:lit, 1]], [:block, [:call, [:self], :blah, [:array]]]],
        [:when, [:array, [:lit, 99]], [:block, [:call, [:self], :bleh, [:array]]]],
        [:else, [:call, [:self], :bluh, [:array]]]
      ],
      nil
    ]
    
    
    got = norm input
    assert_equal output, got
  end
    
  
end
