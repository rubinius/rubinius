require 'test/unit'
require 'translation/case_convert'

class TestCaseConverter < Test::Unit::TestCase
  
  def setup
    @cc = CaseConverter.new
  end
  
  def test_case_into_if
    input = [:case, [:true], [
      [:when, [:array, [:true]], [:true]],
      [:when, [:array, [:lit, 2]], [:false]]
    ]]
    
    output = [:block, 
      [:lasgn, :__uqcs2, 2, [:true]],
      [:if, [:call, [:lvar, :__uqcs2, 2], :===, [:array, [:true]]],
        [:true],
        [:if, [:call, [:lvar, :__uqcs2, 2], :===, [:array, [:lit, 2]]],
          [:false]
          ]
        ]
    ]
    
    got = @cc.process input
    assert_equal output, got
    
  end
  
  def test_case_into_if_with_or
    input = [:case, [:true], [
      [:when, [:array, [:true], [:false]], [:true]],
      [:when, [:array, [:lit, 2]], [:false]]
    ]]
    
    assert !@cc.detect_numbers_only(input)
    @cc.support_switch = true
    
    output = [:block, 
      [:lasgn, :__uqcs2, 2, [:true]],
      [:if, [:or, 
            [:call, [:lvar, :__uqcs2, 2], :===, [:array, [:false]]],
            [:call, [:lvar, :__uqcs2, 2], :===, [:array, [:true]]]
            ],
        [:true],
        [:if, [:call, [:lvar, :__uqcs2, 2], :===, [:array, [:lit, 2]]],
          [:false]
          ]
        ]
    ]
    
    got = @cc.process input
    assert_equal output, got
  end
  
  def test_case_into_switch
    input = [:case, [:true], [
      [:when, [:array, [:lit, 8]], [:true]],
      [:when, [:array, [:lit, 2]], [:false]]
    ]]
    
    @cc.support_switch = true
    
    assert @cc.detect_numbers_only(input)
    
    output = [:switch, [:true], [
      [[:array, [:lit, 8]], [:true]],
      [[:array, [:lit, 2]], [:false]]
    ]]
    
    got = @cc.process input
    assert_equal output, got
  end
end