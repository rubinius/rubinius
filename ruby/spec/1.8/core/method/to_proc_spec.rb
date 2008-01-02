require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#to_proc" do
  before(:each) do
    @m = MethodSpecs::Methods.new
    @meth = @m.method(:foo)
  end

  it "returns a Proc object corresponding to the method" do
    @meth.to_proc.kind_of?(Proc).should == true
  end

  it "Proc object should have the correct arity" do
    # This may seem redundant but this bug has cropped up in jruby, mri and yarv.
    # http://jira.codehaus.org/browse/JRUBY-124
    [ :zero, :one_req, :two_req,
      :zero_with_block, :one_req_with_block, :two_req_with_block,
      :one_opt, :one_req_one_opt, :one_req_two_opt, :two_req_one_opt,
      :one_opt_with_block, :one_req_one_opt_with_block, :one_req_two_opt_with_block, :two_req_one_opt_with_block,
      :zero_with_splat, :one_req_with_splat, :two_req_with_splat,
      :one_req_one_opt_with_splat, :one_req_two_opt_with_splat, :two_req_one_opt_with_splat,
      :zero_with_splat_and_block, :one_req_with_splat_and_block, :two_req_with_splat_and_block,
      :one_req_one_opt_with_splat_and_block, :one_req_two_opt_with_splat_and_block, :two_req_one_opt_with_splat_and_block
    ].each do |m|
      @m.method(m).to_proc.arity.should == @m.method(m).arity
    end
  end
end
