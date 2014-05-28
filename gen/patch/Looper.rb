
c = ::RBe::Cpp::Class["Looper"]
if c
  c.wrapper = true

  # hooks
  c.set_hook "MessageReceived"
  c.set_hook "DispatchMessage"
  c.set_hook "QuitRequested"

  %w(DispatchMessage).each do |name|
    c.functions(name).each do |f|
      f.set_option :custom_hook, true
    end
  end

  # AssertLocked
  %w(IsMessageWaiting
     AddHandler
     RemoveHandler
     CountHandlers
     HandlerAt
     IndexOf
     Run
     Quit
     Unlock
     AddCommonFilter
     RemoveCommonFilter
     SetCommonFilterList
    ).each do |name|
    c.functions(name).each do |f|
      f.set_option :assert_locked, true
    end
  end

  c.functions("QuitRequested").each do |f| f.ret_default = "true" end

  %w(AddHandler RemoveHandler Run Quit Lock LockWithTimeout).each do |s|
    c.functions(s).each do |f|
      f.set_option :custom, true
    end
  end

  %w(DetachCurrentMessage Thread Sem AddCommonFilter RemoveCommonFilter SetCommonFilterList CommonFilterList MessageFromPort DispatchExternalMessage Perform).each do |s|
    c.functions(s).each do |f|
      f.set_option :noimp, true
    end
  end

  [
   ["void *", "fFt", "NULL"],
   ["bool", "fInitialized", "false"],
   ["bool", "fRunCalled", "false"]
  ].each do |type, name, val|
    v = RBe::Cpp::Variable.new(type, name, val)
    c.add_inner_variable v
  end
end
