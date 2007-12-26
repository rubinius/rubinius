require File.dirname(__FILE__) + '/../compiler_helper'

extension :rubinius do
  describe "RsNormalizer, normalizing a :many_if" do
    before do
      @normalizer = RsNormalizer.new(RsLocalState.new, true)
    end
  
    it "expands multiple conditional expressions in a case into a boolean disjunction" do
      input_sexp =
        [:many_if,
          [
            [[:array, [:true], [:false]], 
              [:newline, 248, "spec/language/case_spec.rb",
                [:str, "foo"],
              ]
            ]
          ],
          nil]
    
      expected_sexp =
        [:if, [:or, [:true], [:false]],
          [:newline, 248, "spec/language/case_spec.rb",
            [:str, "foo"]],
          nil]
        
      @normalizer.process(input_sexp).should == expected_sexp
    end
  
    it "normalizes the cases a series of nested if statements" do
      input_sexp =
        [:many_if, 
          [
            [[:array, [:false]], 
              [:newline, 248, "spec/language/case_spec.rb", 
                [:str, "foo"]
              ]
            ], 
            [[:array, [:lit, 2]], 
              [:newline, 249, "spec/language/case_spec.rb", 
                [:str, "bar"]
              ]
            ], 
            [
              [:array, 
                [:call, [:lit, 1], :==, [:array, [:lit, 1]]]
              ], 
              [:newline, 250, "spec/language/case_spec.rb", 
                [:str, "baz"]
              ]
            ]
          ],
          nil
        ]
    
      expected_sexp = 
        [:if, [:false],
          [:newline, 248, "spec/language/case_spec.rb", 
            [:str, "foo"]
          ],
          [:if, [:lit, 2],
            [:newline, 249, "spec/language/case_spec.rb", 
              [:str, "bar"]
            ],
            [:if, [:call, [:lit, 1], :==, [:array, [:lit, 1]], {}],
              [:newline, 250, "spec/language/case_spec.rb", 
                [:str, "baz"]
              ],
              nil]]]
    
      @normalizer.process(input_sexp).should == expected_sexp
    end
  end
end
