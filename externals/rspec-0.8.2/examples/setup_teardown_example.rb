$global = 0

context "State created in context_setup" do
  context_setup do
    @sideeffect = 1
    $global +=1
  end

  setup do
    @isolated = 1
  end
  
  specify "should be accessible from spec" do
    @sideeffect.should == 1
    $global.should == 1
    @isolated.should == 1

    @sideeffect += 1
    @isolated += 1
  end

  specify "should not have sideffects" do
    @sideeffect.should == 1
    $global.should == 1
    @isolated.should == 1

    @sideeffect += 1
    @isolated += 1
  end
  
  context_teardown do
    $global = 0
  end
end