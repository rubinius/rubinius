require File.expand_path(File.dirname(__FILE__) + '/../test_helper')
require 'translation/local_scoping'

class TestLocalScoping < Test::Unit::TestCase
  
  def setup
    @pr = RsLocalScoper.new
  end
  
  def trans(i)
    @pr.process i
  end
  
  # Test that all d[asgn|asgn_cur|var] commands go away.
  def test_block_vars_as_locals
    sx = [:iter, [:fcall, :blah], 
      [:masgn, [:array, [:dasgn_curr, :a]], nil, nil],
      [:dvar, :a]]
    
    expected = [:iter, [:fcall, :blah],
      [:masgn, [:array, [:lasgn, :a, 2]], nil, nil],
      [:lvar, :a, 2]
    ]
    
    out = trans sx
    
    assert_equal expected, out
  end
  
  # Test that a block arg doesn't make it out of the scope
  # it's an argument for.
  def test_block_args_dont_go_up
    sx = [:block,
      [:iter, [:fcall, :blah],
      [:masgn, [:array, [:dasgn_curr, :a]], nil, nil],
      nil
      ],
      [:vcall, :a]
    ]
    
    expected = [:block,
      [:iter, [:fcall, :blah],
      [:masgn, [:array, [:lasgn, :a, 2]], nil, nil],
      nil
      ],
      [:vcall, :a]
    ]
    
    out = trans sx
    assert_equal expected, out
  end
  
  # Test that normal var assignment inside a block (what was
  # called dasgn_curr or dasgn) turns into a lasgn that's still
  # visible after the block exits.
  def test_inner_vars_are_outer_local    
    sx = [:block,
      [:iter, [:fcall, :blah],
        nil,
        [:block, 
          [:dasgn_curr, :a, [:lit, 9]],
          [:dasgn, :b, [:lit, 1]]
        ]
      ],
      [:vcall, :a],
      [:vcall, :b]
    ]
    
    expected = [:block,
      [:iter, [:fcall, :blah],
        nil,
        [:block, 
          [:lasgn, :a, 2, [:lit, 9]],
          [:lasgn, :b, 3, [:lit, 1]]
        ]
      ],
      [:lvar, :a, 2],
      [:lvar, :b, 3]
    ]
    
    out = trans sx
    assert_equal expected, out
  end
  
  # Test the block arguments are only scoped down into their
  # defining scope, and that even if they have overlapping names,
  # their assignment slot is not reused.
  def test_args_only_go_down
    sx = [:iter,
     [:fcall, :m],
     [:dasgn_curr, :a],
     [:block,
      [:iter, [:fcall, :l], [:dasgn_curr, :b], [:lit, 1]],
      [:iter, [:fcall, :g], [:dasgn_curr, :b], [:lit, 2]]]]
    
    expected = [:iter,
       [:fcall, :m],
       [:lasgn, :a, 2],
       [:block,
        [:iter, [:fcall, :l], [:lasgn, :b, 3], [:lit, 1]],
        [:iter, [:fcall, :g], [:lasgn, :b, 4], [:lit, 2]]]]
    
    out = trans sx
    assert_equal expected, out
  end
  
  def test_block_args_can_be_shadowed
    sx = [:block,
     [:lasgn, :a, 2, [:lit, 1]],
     [:iter, [:fcall, :blah], [:lasgn, :a, 2], [:nil]],
     [:fcall, :p, [:array, [:lvar, :a, 2]]]]
    
    expected =  [:block,
      [:lasgn, :a, 2, [:lit, 1]],
      [:iter, [:fcall, :blah], [:lasgn, :a, 2], [:nil]],
      [:fcall, :p, [:array, [:lvar, :a, 2]]]]
        
    out = trans sx
    assert_equal expected, out
  end
end